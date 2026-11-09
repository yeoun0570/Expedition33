// 명령어		옵션		원본 파일이 있는 위치		복사한 파일을 저장할 위치

xcopy		/y/s	.\Engine\Public\*.h			.\EngineSDK\Inc\
xcopy		/y		.\Engine\Bin\Engine.lib		.\EngineSDK\Lib\
xcopy		/y		.\Engine\Shader\*.hlsl		.\EngineSDK\Shader\
xcopy		/y		.\Engine\Bin\Engine.dll		.\Client\Bin\