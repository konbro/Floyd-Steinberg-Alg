#ifndef EZ_CONNECT_H
#define EZ_CONNECT_H
#include <string>
#include <Windows.h>
#include "Bmp.h"
class ez_connect
{
public:

	
	typedef int(__cdecl* TransformPixelAsmFunction)(uint8_t gray, uint32_t bw_threshold);

	//static TransformPixelAsmFunction TPAF;
	TransformPixelAsmFunction TPAF;

	//static HMODULE hmodule_asm_link;
	HMODULE hmodule_asm_link;
	//inline static void loadDLLAsm()
	void loadDLLAsm()
	{
		hmodule_asm_link = LoadLibraryA("AsmLibrary");
		if (hmodule_asm_link != 0 && hmodule_asm_link != NULL)
		{
			ez_connect::TPAF = (ez_connect::TransformPixelAsmFunction)GetProcAddress(hmodule_asm_link, "TransformPixelAsm");
		}
	}
	ez_connect::ez_connect() {};

};
#endif