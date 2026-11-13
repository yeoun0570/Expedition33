// cbuffer : 상수 버퍼의 데이터 구성을 선언

// register(b0) : 이 데이터를 상수 버퍼 슬롯 0번에서 읽겠다는 지정
// Shader.cpp의 Bind_Shader()에서 0번 상수 버퍼 번호에 연결했으니 0번을 뜻하는 b0을 사용


// 하나의 상수 버퍼만 사용할거니까 0번만 둘거임
// 예를 들어, 베르소의 몸체와 얼굴 메시가 똑같은 월드,뷰,투영 행렬 속에서 있을 거니까 굳이 상수 버퍼가 여러 개 있을 이유가 없음
cbuffer TransformBuffer : register(b0)
{
    // row_major : 행렬을 행 순서로 저장한다는 뜻
    
    row_major float4x4 g_matWorld;
    row_major float4x4 g_matView;
    row_major float4x4 g_matProj;
};

// 정점 셰이더가 받는 데이터
// VS_INPUT : 내가 정한 구조체 이름, VS는 Vertex Shader의 약자
struct VS_INPUT
{
    // float3 : 실수 성분 3개: 위치의 X·Y·Z
    // float4 : 실수 성분 4개: 색상의 R·G·B·A
    // : POSITION, : COLOR : 시맨틱(이름표)
    
    float3 vPosition    : POSITION; 
    float4 vColor       : COLOR;          
};


// 정점 셰이더가 받는 데이터
struct VS_OUTPUT
{
    // SV : System Value의 약자로 DX에서 정해 둔 이름을 사용해야 한다.
    // GPU에게 SV가 붙여진 녀석으로 화면상 위치를 결정하라고 알려주는 거임.
    float4 vPosition    : SV_Position;
    float4 vColor       : COLOR;
};

// VS_MAIN은 함수이름, 정점 셰이더 함수
// 즉, VS_OUPUT은 반환 타입, ()안에 있는 VS_INPUT은 매개변수
// 정점마다 실행하는 함수
VS_OUTPUT VS_MAIN(VS_INPUT Input)
{
    VS_OUTPUT Output;
    
    float4 vPosition = float4(Input.vPosition, 1.f);

    // mul : HLSL에서 벡터와 행렬 등을 곱하는 함수
    // vPosition에 월드, 뷰, 투영 행렬을 곱해서 해당 스페이스 만들기
    vPosition = mul(vPosition, g_matWorld);
    vPosition = mul(vPosition, g_matView);
    vPosition = mul(vPosition, g_matProj);

    Output.vPosition = vPosition;
    Output.vColor = Input.vColor;
    
    return Output;
}

// PS_MAIN함수는 픽셀 셰이더 함수
// SV_TARGET도 그냥 시맨틱(이름표)
// SV_TARGET은 반환값이 출력 대상에 기록할 색상인 것을 알려줌
float4 PS_MAIN(VS_OUTPUT Output) : SV_TARGET
{
    return Output.vColor;
}