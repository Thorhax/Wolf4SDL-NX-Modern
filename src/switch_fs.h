#ifndef _SWITCH_FS_H_
#define _SWITCH_FS_H_

#ifdef __SWITCH__
#include <switch.h>
#include <stddef.h>

extern char datadir[256];

void Switch_InitFS();
void Switch_ExitFS();
bool Switch_FindGameFile(char *outpath, size_t outsize, const char *base, const char *ext);
int Switch_OpenGameFile(const char *base, const char *ext, int flags);

#endif
#endif
