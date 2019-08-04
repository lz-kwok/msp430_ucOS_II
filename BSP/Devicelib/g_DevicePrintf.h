#ifndef   G_DEVICEPRINTF_H
#define   G_DEVICEPRINTF_H

#include <stdbool.h>






typedef void (*am_util_stdio_print_char_t)(char *pcStr);
void am_util_stdio_printf_init(am_util_stdio_print_char_t pfnCharPrint);
uint32_t User_Printf(char *pcFmt, ...);
uint32_t g_Printf_info(char *pcFmt, ...);
uint32_t g_Printf_dbg(char *pcFmt, ...);



#endif
