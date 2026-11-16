#include "Model.h"
#include <set>


CModel::CModel(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext)
    : CComponent { pDevice, pContext }
{
}

HRESULT CModel::Initialize_Prototype(const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas, const map<string, string>& JointMap)
{
    if (m_pDevice == nullptr || m_pContext == nullptr)
        return E_FAIL;

    if (MeshDatas.empty() && NodeDatas.empty())
        return E_FAIL;

    // 메시가 없는 모델도 노드가 저장돼 있으면 이미 초기화된 상태다.
    if (!m_Meshes.empty() || !m_NodeDatas.empty())
        return E_FAIL;

    /* Fbx VIBuffer 메시로 만들고 저장 */

    for (const MESH_DATA& MeshData : MeshDatas)
    {
        m_Meshes.push_back(CVIBuffer_Mesh::Create(m_pDevice, m_pContext, MeshData));

        if (m_Meshes.back() == nullptr)
        {
            m_Meshes.pop_back();
            return E_FAIL;
        }
    }

    m_NodeDatas = NodeDatas;
    m_MeshDatas = MeshDatas;
    m_JointMap = JointMap;

    /* 노드와 클러스터 연결 */
    map<uint64_t, int32_t>  NodeIndicies;

    for (uint64_t iNode = 0; iNode < m_NodeDatas.size(); ++iNode)
    {
        NodeIndicies.emplace(m_NodeDatas[iNode].iNodeID, static_cast<int32_t>(iNode));
    }


    for (MESH_DATA& MeshDatas : m_MeshDatas)
    {
        for (FBX_SKIN_DATA& SkinDatas : MeshDatas.FbxSkins)
        {
            for (FBX_CLUSTER_DATA& ClusterDatas : SkinDatas.FbxClusters)
            {
                auto    iter = NodeIndicies.find(ClusterDatas.iBoneID);
                if (iter == NodeIndicies.end())
                    return E_FAIL;

                ClusterDatas.iNodeIndex = iter->second;
            }
        }
    }

    /* 뼈 매핑을 위한 노드에 뼈 이름 저장 */
    for (uint64_t iNode = 0; iNode < m_NodeDatas.size(); ++iNode)
    {
        FBX_NODE_DATA& NodeData = m_NodeDatas[iNode];
        if (!NodeData.isBone)
            continue;

        auto    iter = m_BoneIndicies.emplace(NodeData.strName, static_cast<int32_t>(iNode));
        if (!iter.second)
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    // 모델 복사 직후에는 원본의 메시 객체들을 함께 참조합니다.
    // 각 메시를 복제해서 사본 전용 목록을 만듭니다.
    vector<shared_ptr<CVIBuffer_Mesh>> Meshes;
    Meshes.reserve(m_Meshes.size());

    for (const auto& pMesh : m_Meshes)
    {
        if (nullptr == pMesh)
            return E_FAIL;

        // 메시의 Initialize()가 사본 전용 정점 버퍼를 만듭니다.
        auto pMeshClone = dynamic_pointer_cast<CVIBuffer_Mesh>(pMesh->Clone(nullptr));

        if (nullptr == pMeshClone)
            return E_FAIL;

        Meshes.push_back(pMeshClone);
    }

    // 모든 메시 복제에 성공한 뒤 사본의 목록을 교체합니다.
    m_Meshes.swap(Meshes);

    // 사본의 조립 상태는 원형에서 물려받지 않고 새로 준비합니다.
    m_SourceNodeIndices.clear();
    m_NodeMatrices.clear();

    // 사본이 원본의 조립된 파츠를 그대로 공유하지 않도록 비운다.
    m_Parts.clear();

    return S_OK;
}

void CModel::Render()
{
    for (const auto& pMesh : m_Meshes)
    {
        pMesh->Begin();
        pMesh->Render();
        pMesh->End();
    }

    // 전체 모델이 보관한 파츠들을 차례대로 그린다.
    for (const auto& Part : m_Parts)
    {
        // 이름은 first, 해당 파츠의 모델은 second에 들어 있다.
        Part.second->Render();
    }
}

HRESULT CModel::Assemble(const CModel& SourceModel,const map<wstring_t, shared_ptr<CModel>>& PartModels)
{
    vector<shared_ptr<CModel>> Parts;

    // 한 항목에서 파츠 이름과 모델을 함께 꺼낸다.
    for (const auto& [strPartName, pPart] : PartModels)
    {
        // 교체할 파츠를 구분할 수 있도록 이름이 있어야 한다.
        if (strPartName.empty())
            return E_FAIL;

        // 없는 모델이나 전체 모델 자신을 파츠로 넣지 못하게 한다.
        if (nullptr == pPart || pPart.get() == this)
            return E_FAIL;

        // 파츠가 다른 파츠를 소유하는 중첩 연결을 막는다.
        if (!pPart->m_Parts.empty())
            return E_FAIL;

        // 다른 이름으로 같은 모델을 중복 등록하는 것도 막는다.
        if (find(Parts.begin(), Parts.end(), pPart) != Parts.end())
            return E_FAIL;

        Parts.push_back(pPart);
    }

    // 각 파츠에서 기준 모델과 같은 이름의 뼈를 찾아,
    // 정점 계산에 사용할 기본 자세를 준비한다.
    for (const auto& pPart : Parts)
    {
        // 각 파츠를 별도로 전달받은 기준 뼈대에 연결한다.
        if (FAILED(pPart->Bind_Source(SourceModel)))
            return E_FAIL;
    }

    // 출력할 파츠들만 정점 계산 대상으로 넣는다.
    // 몸체도 PartModels로 전달받는다.
    for (const auto& pPart : Parts)
    {
        if (FAILED(pPart->Update_MeshVertices()))
            return E_FAIL;
    }

    // 계산이 끝나면 이름과 모델을 함께 보관한다.
    m_Parts = PartModels;

    return S_OK;
}

HRESULT CModel::Set_Animation(const shared_ptr<FBX_ANIM_DATA>& pAnimData)
{
    /* 재생할 클립 지정 + 뼈 이름으로 채널↔노드 연결 */
    if (nullptr == pAnimData || 0 == pAnimData->iFrameCount)
        return E_FAIL;

    vector<int32_t> ChannelNodeIndices(pAnimData->Channels.size(), -1);

    for (size_t iChannel = 0; iChannel < pAnimData->Channels.size(); ++iChannel)
    {
        ChannelNodeIndices[iChannel] = Find_BoneIndex(pAnimData->Channels[iChannel].strBoneName);
    }

    m_pAnimData = pAnimData;
    m_ChannelNodeIndices.swap(ChannelNodeIndices);

    // 새 동작은 처음부터 재생
    m_fAnimTime = 0.0;

#ifdef _DEBUG
    {
        // 공통 뼈대에서 애니메이션 채널이 있는 노드 표시
        vector<bool_t> IsAnimated(m_NodeDatas.size(), false);
        for (int32_t iNode : m_ChannelNodeIndices)
        {
            if (iNode >= 0)
                IsAnimated[iNode] = true;
        }

        char_t szLog[256]{};

        for (const auto& [strPartName, pPart] : m_Parts)
        {
            set<string> Unmapped;     // ① 공통 뼈대에 연결 안 된 뼈
            set<string> NotAnimated;  // ② 연결은 됐지만 애니메이션이 없는 뼈

            for (const MESH_DATA& MeshData : pPart->m_MeshDatas)
            {
                for (const FBX_SKIN_DATA& SkinData : MeshData.FbxSkins)
                {
                    for (const FBX_CLUSTER_DATA& ClusterData : SkinData.FbxClusters)
                    {
                        const int32_t iSource = pPart->m_SourceNodeIndices[ClusterData.iNodeIndex];

                        if (iSource < 0)
                            Unmapped.insert(ClusterData.strBoneName);
                        else if (!IsAnimated[iSource])
                            NotAnimated.insert(ClusterData.strBoneName);
                    }
                }
            }

            sprintf_s(szLog, "[Seam] %ls: 연결 안 됨 %zu개, 애니메이션 없음 %zu개\n",
                strPartName.c_str(), Unmapped.size(), NotAnimated.size());
            OutputDebugStringA(szLog);

            for (const string& strName : Unmapped)
            {
                sprintf_s(szLog, "    [연결 안 됨] %s\n", strName.c_str());
                OutputDebugStringA(szLog);
            }

            for (const string& strName : NotAnimated)
            {
                sprintf_s(szLog, "    [애니메이션 없음] %s\n", strName.c_str());
                OutputDebugStringA(szLog);
            }
        }
    }
#endif

    return S_OK;
}

HRESULT CModel::Play_Animation(f64_t fTimeDelta)
{
    /* 조립 모델: 재생 시간 진행 → 공통 뼈대 자세 계산 → 파츠에 전달 */

    if (nullptr == m_pAnimData || m_pAnimData->fDuration <= 0.0)
        return E_FAIL;

    // 1. 재생 위치 진행 (끝나면 처음으로 돌아가 반복 재생)
    m_fAnimTime += fTimeDelta;
    if (m_fAnimTime >= m_pAnimData->fDuration)
        m_fAnimTime -= m_pAnimData->fDuration;

    // 2. 지금 시간의 로컬 → 글로벌
    vector<float4x4_t> LocalMatrices;

    if (FAILED(Sample_LocalMatrices(m_fAnimTime, LocalMatrices)))
        return E_FAIL;

    if (FAILED(Calculate_GlobalMatrices(LocalMatrices, m_GlobalMatrices)))
        return E_FAIL;

    // 3. 각 파츠에 자세 전달 + 정점 다시 계산
    for (const auto& [strPartName, pPart] : m_Parts)
    {
        if (FAILED(pPart->Apply_SourcePose(m_GlobalMatrices)))
            return E_FAIL;
    }

    return S_OK;
}

void CModel::Build_BoneMapping(const CModel& SourceModel, vector<int32_t>& SourceNodeIndices) const
{
    /* 뼈 매핑을 위해서 서로 다른 Fbx모델의 똑같은 뼈 이름 찾기 */

    // 공통 뼈가 없는 경우는 -1로 처리하기 위해서 일단 뼈 개수만큼 -1로 vector를 전부 채우기
    SourceNodeIndices.assign(m_NodeDatas.size(), -1);

    for (uint64_t iNode = 0; iNode < m_NodeDatas.size(); ++iNode)
    {
        const FBX_NODE_DATA& NodeData = m_NodeDatas[iNode];
        if (!NodeData.isBone)
            continue;

        // 매핑 표에 있으면 표가 가리키는 뼈를, 없으면 같은 이름의 뼈를 따라간다
        auto iter = m_JointMap.find(NodeData.strName);
        const string& strSourceName = (iter != m_JointMap.end()) ? iter->second : NodeData.strName;

        SourceNodeIndices[iNode] = SourceModel.Find_BoneIndex(strSourceName);

#ifdef _DEBUG
        // 이름이 다른 쌍(표로 연결된 뼈)만 기준 자세 위치 비교
        if (iter != m_JointMap.end() && iter->second != NodeData.strName && SourceNodeIndices[iNode] >= 0)
        {
            const auto& Mine = NodeData.GlobalMatrix;                                        // 얼굴 FBX의 이 뼈
            const auto& Src = SourceModel.m_NodeDatas[SourceNodeIndices[iNode]].GlobalMatrix; // 몸체 FBX의 원래 뼈

            f64_t fMaxDiff = 0.0;
            for (int32_t iRow = 0; iRow < 4; ++iRow)
            {
                for (int32_t iCol = 0; iCol < 4; ++iCol)
                    fMaxDiff = max(fMaxDiff, fabs(Mine[iRow][iCol] - Src[iRow][iCol]));
            }

            char_t szLog[256]{};
            sprintf_s(szLog, "[Bind] %s -> %s : 최대 차이 %f, 위치(%.4f %.4f %.4f) vs (%.4f %.4f %.4f)\n",
                NodeData.strName.c_str(), strSourceName.c_str(), fMaxDiff,
                Mine[3][0], Mine[3][1], Mine[3][2],
                Src[3][0], Src[3][1], Src[3][2]);
            OutputDebugStringA(szLog);
        }
#endif
    }
}

HRESULT CModel::Build_DefaultPose(const CModel& SourceModel, const vector<int32_t>& SourceNodeIndices, vector<float4x4_t>& NodeMatrices) const
{
    /* 뼈 매핑 : 대응되는 번호, 즉 공통 뼈들을 글로벌 행렬 가져와서 저장 */

    if (SourceNodeIndices.size() != m_NodeDatas.size())
        return E_FAIL;

    vector<float4x4_t> Matrices(m_NodeDatas.size());

    for (size_t iNode = 0; iNode < m_NodeDatas.size(); ++iNode)
    {
        const int32_t iSourceIndex = SourceNodeIndices[iNode];
        const FBX_NODE_DATA* pPoseNode = &m_NodeDatas[iNode];

        if (iSourceIndex >= 0)
        {
            if (static_cast<size_t>(iSourceIndex) >= SourceModel.m_NodeDatas.size())
                return E_FAIL;

            pPoseNode = &SourceModel.m_NodeDatas[iSourceIndex];
        }

        for (int32_t iRow = 0; iRow < 4; ++iRow)
        {
            for (int32_t iCol = 0; iCol < 4; ++iCol)
            {
                Matrices[iNode].m[iRow][iCol] = static_cast<f32_t>(pPoseNode->GlobalMatrix[iRow][iCol]);
            }
        }
    }

    // NodeMatrices에다가 부모 뼈의 기본 자세 글로벌 행렬인 m_NodeDatas를 저장해놓는다.
    // 그러면 이제 NodeMatrices를 가지고 애니메이션에 따른 현재 자세를 계산하는데 사용할 것이다.
    NodeMatrices.swap(Matrices);
    return S_OK;
}

HRESULT CModel::Bind_Source(const CModel& SourceModel)
{
    vector<int32_t> SourceNodeIndices;
    vector<float4x4_t> NodeMatrices;

    Build_BoneMapping(SourceModel, SourceNodeIndices);

    if (FAILED(Build_DefaultPose(SourceModel, SourceNodeIndices, NodeMatrices)))
        return E_FAIL;
    

    m_SourceNodeIndices.swap(SourceNodeIndices);
    m_NodeMatrices.swap(NodeMatrices);

    return S_OK;
}

HRESULT CModel::Calculate_ClusterMatrix(const FBX_CLUSTER_DATA& ClusterData, const float4x4_t& NodeMatrix, float4x4_t& SkinMatrix) const
{
    /* 글로벌 행렬에다가 바인드 행렬까지 반영해서 뼈가 움직였을 때 정점 위치들 계산 */

    FbxMatrix MeshBind;
    FbxMatrix BoneBind;
    FbxMatrix BoneCurrent;

    // 저장한 행렬 값을 FBX SDK의 계산용 행렬에 복사
    for (int32_t iRow = 0; iRow < 4; ++iRow)
    {
        for (int32_t iCol = 0; iCol < 4; ++iCol)
        {
            MeshBind.Set(iRow, iCol, ClusterData.MeshBindMatrix[iRow][iCol]);

            BoneBind.Set(iRow, iCol, ClusterData.BoneBindMatrix[iRow][iCol]);

            BoneCurrent.Set(iRow, iCol, NodeMatrix.m[iRow][iCol]);
        }
    }

    // 역행렬을 만들 수 없는 입력이면 계산을 중단
    if (BoneBind.Determinant() == 0.0)
        return E_FAIL;

    // 고정된 바인드 기준(MeshBind, BoneBind)과 현재 뼈 자세(BoneCurrent)를 결합해 원본 정점을 현재 자세로 옮길 행렬을 만든다.
    // BoneBind가 역행렬을 사용하는 이유는 BoneBind는 fbx에서 해당 뼈의 좌표(위치, 크기, 회전)를 알려주는 건데, 여기서 필요한건 뼈 기준으로 정점의 좌표(위치, 크기, 회전)를 구하는 것이기 때문이다.
    const FbxMatrix Result = BoneCurrent * BoneBind.Inverse() * MeshBind;

    for (int32_t iRow = 0; iRow < 4; ++iRow)
    {
        for (int32_t iCol = 0; iCol < 4; ++iCol)
        {
            SkinMatrix.m[iRow][iCol] = static_cast<f32_t>(Result.Get(iRow, iCol));
        }
    }

    return S_OK;
}

HRESULT CModel::Calculate_MeshVertices(uint32_t iMeshIndex, vector<VTXCOL>& Vertices) const
{
    /* 정점들의 가중치 가져와서 반영하기 */

    // 요청한 메시 번호가 실제로 있는지 확인합니다.
    if (iMeshIndex >= m_MeshDatas.size())
        return E_FAIL;

    // 계산에 사용할 원본 메시를 가져옵니다.
    const MESH_DATA& MeshData = m_MeshDatas[iMeshIndex];
    const size_t iPointCount = MeshData.Verticles.size();

    // 각 정점의 위치 계산 결과를 더해둘 공간입니다.
    vector<FbxVector4> PositionSums(iPointCount, FbxVector4(0.0, 0.0, 0.0, 0.0));

    // 각 정점에 사용한 가중치를 더해둘 공간입니다.
    vector<f64_t> WeightSums(iPointCount, 0.0);

    
    /* ============================================================================================ = */


    if (MeshData.FbxSkins.size() != 1)
        return E_FAIL;

    const FBX_SKIN_DATA& SkinData = MeshData.FbxSkins[0];

    // 이 스킨에 들어 있는 모든 클러스터를 읽습니다.
    for (const FBX_CLUSTER_DATA& ClusterData : SkinData.FbxClusters)
    {
        // 이 클러스터가 연결된 뼈의 노드 번호입니다.
        const int32_t iNode = ClusterData.iNodeIndex;

        // 해당 뼈의 자세가 준비되어 있는지 확인합니다.
        if (iNode < 0 || static_cast<size_t>(iNode) >= m_NodeMatrices.size())
            return E_FAIL;

        // 바인드 기준과 현재 뼈 자세로 정점 변환 행렬을 만듭니다.
        float4x4_t Matrix{};

        if (FAILED(Calculate_ClusterMatrix(ClusterData, m_NodeMatrices[iNode], Matrix)))
            return E_FAIL;
        

        // 아래의 정점 변환에는 기존 SDK 행렬을 사용한다.
        // SDK 형식은 구현 파일 안에서만 사용한다.
        FbxMatrix SkinMatrix;

        for (int32_t iRow = 0; iRow < 4; ++iRow)
        {
            for (int32_t iCol = 0; iCol < 4; ++iCol)
            {
                SkinMatrix.Set(iRow, iCol, Matrix.m[iRow][iCol]);
            }
        }


        /* ============================================================================================ = */


        // 현재 클러스터에 연결된 정점 번호와 가중치를 하나씩 읽습니다.
        for (const FBX_POINT_WEIGHT& PointWeight : ClusterData.FbxPointWeights)
        {
            const int32_t iPoint = PointWeight.iPointIndex;

            // 원본 정점 배열에 접근할 수 있는 번호인지 검사합니다.
            if (iPoint < 0 || static_cast<size_t>(iPoint) >= iPointCount)
                return E_FAIL;

            // 매번 원본 위치에서 계산합니다.
            const float3_t& Original = MeshData.Verticles[iPoint].vPosition;

            // 이 뼈를 100% 따라갔을 때의 위치를 계산합니다.
            const FbxVector4 Position = SkinMatrix.MultNormalize(FbxVector4(Original.x, Original.y, Original.z, 1.0));

            // 계산한 위치에 저장된 가중치를 곱해서 더합니다.
            PositionSums[iPoint] += Position * PointWeight.fWeight;

            // 마지막에 나눌 가중치 합도 함께 더합니다.
            WeightSums[iPoint] += PointWeight.fWeight;
        }
    }

    /* ============================================================================================ = */

    // 색상 등 기존 정점 정보는 복사하고, 위치만 교체합니다.
    vector<VTXCOL> Results = MeshData.Verticles;

    for (size_t iPoint = 0; iPoint < iPointCount; ++iPoint)
    {
        // 이번 구현은 모든 정점에 유효한 가중치가 있어야 합니다.
        if (WeightSums[iPoint] <= 0.0)
            return E_FAIL;

        // 누적한 위치를 가중치 합으로 나눕니다.
        const FbxVector4 Position = PositionSums[iPoint] / WeightSums[iPoint];

        Results[iPoint].vPosition =
        {
            static_cast<f32_t>(Position[0]),
            static_cast<f32_t>(Position[1]),
            static_cast<f32_t>(Position[2])
        };
    }

    // 모든 정점 계산이 성공한 경우에만 출력 목록을 교체합니다.
    Vertices.swap(Results);

    return S_OK;
}

HRESULT CModel::Calculate_GlobalMatrices(const vector<float4x4_t>& LocalMatrices, vector<float4x4_t>& GlobalMatrices) const
{
    /* 로컬 행렬 → 글로벌 행렬 (로컬 * 부모 글로벌) */

    if (LocalMatrices.size() != m_NodeDatas.size())
        return E_FAIL;

    GlobalMatrices.resize(m_NodeDatas.size());

    for (uint64_t iNode = 0; iNode < m_NodeDatas.size(); ++iNode)
    {
        const matrix_t LocalMatrix = XMLoadFloat4x4(&LocalMatrices[iNode]);
        const int32_t  iParent = m_NodeDatas[iNode].iParentIndex;

        // 루트는 부모가 없으니 로컬이 곧 글로벌
        if (iParent < 0)
        {
            XMStoreFloat4x4(&GlobalMatrices[iNode], LocalMatrix);
            continue;
        }

        // 트리구조라서 부모 번호가 자식 번호보다 작아야 한다
        if (static_cast<uint64_t>(iParent) >= iNode)
            return E_FAIL;

        const matrix_t ParentGlobalMatrix = XMLoadFloat4x4(&GlobalMatrices[iParent]);

        // 내 글로벌 = 내 로컬 × 부모 글로벌
        XMStoreFloat4x4(&GlobalMatrices[iNode], XMMatrixMultiply(LocalMatrix, ParentGlobalMatrix));
    }

    return S_OK;
}

void CModel::Get_DefaultLocalMatrices(vector<float4x4_t>& LocalMatrices) const
{
    /* FBX 기본 자세의 로컬 행렬 목록 */
    LocalMatrices.resize(m_NodeDatas.size());

    for (uint64_t iNode = 0; iNode < m_NodeDatas.size(); ++iNode)
    {
        for (int32_t iRow = 0; iRow < 4; ++iRow)
        {
            for (int32_t iCol = 0; iCol < 4; ++iCol)
            {
                LocalMatrices[iNode].m[iRow][iCol] = static_cast<f32_t>(m_NodeDatas[iNode].LocalMatrix[iRow][iCol]);
            }
        }
    }

}

HRESULT CModel::Sample_LocalMatrices(f64_t fTime, vector<float4x4_t>& LocalMatrices) const
{
    /* t초의 로컬 행렬 목록 (모델 노드 순서) */

    if (nullptr == m_pAnimData || 0 == m_pAnimData->iFrameCount)
        return E_FAIL;

    // 1. 애니메이션에 없는 노드를 위해 기본 자세로 먼저 채움
    Get_DefaultLocalMatrices(LocalMatrices);

    // 2. 시간 → 칸 번호 (일단 보간 없이 앞 칸 사용)
    uint32_t iFrame = static_cast<uint32_t>(fTime * m_pAnimData->fSampleRate);
    iFrame = min(iFrame, m_pAnimData->iFrameCount - 1);

    // 3. 채널(t초 로컬 행렬) 값을 연결된 노드 자리에 덮어쓰기
    for (size_t iChannel = 0; iChannel < m_pAnimData->Channels.size(); ++iChannel)
    {
        const int32_t iNode = m_ChannelNodeIndices[iChannel];
        if (iNode < 0)
            continue;

        LocalMatrices[iNode] = m_pAnimData->Channels[iChannel].LocalMatrices[iFrame];
    }

    return S_OK;
}

HRESULT CModel::Apply_SourcePose(const vector<float4x4_t>& SourceGlobalMatrices)
{
    /* 파츠: 뼈 매핑표대로 공통 뼈대 자세를 내 뼈 자리에 복사 */
    
    if (m_SourceNodeIndices.size() != m_NodeMatrices.size())
        return E_FAIL;

    for (uint64_t iNode = 0; iNode < m_NodeMatrices.size(); ++iNode)
    {
        const int32_t iSourceIndex = m_SourceNodeIndices[iNode];

        // 공통 뼈대에 없는 뼈는 Build_DefaultPose에서 넣은 기본 자세 유지
        if (iSourceIndex < 0)
            continue;

        if (static_cast<uint64_t>(iSourceIndex) >= SourceGlobalMatrices.size())
            return E_FAIL;

        m_NodeMatrices[iNode] = SourceGlobalMatrices[iSourceIndex];
    }

    if (FAILED(Update_MeshVertices()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Update_MeshVertices()
{
    /* 현재 m_NodeMatrices 자세로 모든 메시 정점을 다시 계산해 버퍼에 반영 */

    if (m_MeshDatas.size() != m_Meshes.size())
        return E_FAIL;

    vector<VTXCOL> Vertices;

    for (uint32_t iMesh = 0; iMesh < m_Meshes.size(); ++iMesh)
    {
        if (nullptr == m_Meshes[iMesh])
            return E_FAIL;

        if (FAILED(Calculate_MeshVertices(iMesh, Vertices)))
            return E_FAIL;

        if (FAILED(m_Meshes[iMesh]->Update_Vertices(Vertices)))
            return E_FAIL;
    }

    return S_OK;
}

int32_t CModel::Find_BoneIndex(const string& strBoneName) const
{
    auto    iter = m_BoneIndicies.find(strBoneName);
    if (iter == m_BoneIndicies.end())
        return -1;

    return iter->second;
}

shared_ptr<CPrototype> CModel::Clone(void* pArg)
{
    auto    pInstance = shared_ptr<CModel>(new CModel(*this));

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CModel");
        pInstance.reset();
    }

    return pInstance;
}

shared_ptr<CModel> CModel::Create(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pContext, const vector<MESH_DATA>& MeshDatas, const vector<FBX_NODE_DATA>& NodeDatas, const map<string, string>& JointMap)
{
    auto pInstance = shared_ptr<CModel>(new CModel(pDevice, pContext));

    if (FAILED(pInstance->Initialize_Prototype(MeshDatas, NodeDatas, JointMap)))
    {
        MSG_BOX("Failed to Created : CModel");
        pInstance.reset();
    }

    return pInstance;
}
