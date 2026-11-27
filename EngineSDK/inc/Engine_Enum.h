#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN, END };

	enum class RENDERGROUP { PRIORITY, NONBLEND, BLEND, UI, END };

	enum class STATE { RIGHT, UP, LOOK, POSITION, END };

	enum class D3DTS { VIEW, PROJ, END };

	enum class V_SLOT { OBJECT, FRAME, END };

	enum class P_SLOT { LINEAR_SAMPLER, POINT_SAMPLER };

	//enum INFO {	INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POS, INFO_END };

	//enum ROTATION { ROT_X, ROT_Y, ROT_Z, ROT_END };

	//enum TEXTUREID { TEX_NORMAL, TEX_CUBE, TEX_END };

	//enum RENDERID { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_UI, RENDER_END };

	enum class DIMK { LBUTTON, RBUTTON, WHEEL, END };
		 
	enum class DIMM { X, Y, Z, END };

}
#endif // Engine_Enum_h__
