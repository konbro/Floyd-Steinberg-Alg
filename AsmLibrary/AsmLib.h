#ifdef ASMLIBRARY_EXPORTS
#define ASMLIBRARY_API __declspec(dllexport)
#else
#define ASMLIBRARY_API __declspec(dllimport)
#endif
#include <cstdint>

												//RCX					RDX
extern "C" ASMLIBRARY_API int TransformPixelAsm(uint8_t gray, uint32_t bw_threshold);