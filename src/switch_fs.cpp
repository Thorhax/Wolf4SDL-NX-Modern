#ifdef __SWITCH__
#include <switch.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include "wl_def.h"
#include "switch_fs.h"

static bool fs_initialized = false;
char datadir[256] = "";

static const char *search_dirs[] = {
    "",                         // current directory (where NRO is)
    "/switch/wolf4sdl/",
    "/switch/wolf3d/",
    "sdmc:/switch/wolf4sdl/",
    "sdmc:/switch/wolf3d/",
    "romfs:/",
    "romfs:/wolf3d/",
    NULL
};

void Switch_InitFS()
{
    if (fs_initialized)
        return;
    fs_initialized = true;

    romfsInit();

    // Ensure config/save directory exists on SD
    mkdir("/switch", 0777);
    mkdir("/switch/wolf4sdl", 0777);
    if (configdir[0] == 0)
    {
        strcpy(configdir, "/switch/wolf4sdl");
    }
}

void Switch_ExitFS()
{
    if (!fs_initialized)
        return;
    romfsExit();
    fs_initialized = false;
}

static void ToUpperStr(char *dst, const char *src, size_t maxlen)
{
    size_t i = 0;
    while (src[i] && i < maxlen - 1)
    {
        dst[i] = (char)toupper((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

static void ToLowerStr(char *dst, const char *src, size_t maxlen)
{
    size_t i = 0;
    while (src[i] && i < maxlen - 1)
    {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

bool Switch_FindGameFile(char *outpath, size_t outsize, const char *base, const char *ext)
{
    Switch_InitFS();

    char base_lower[64], base_upper[64];
    char ext_lower[16], ext_upper[16];

    ToLowerStr(base_lower, base, sizeof(base_lower));
    ToUpperStr(base_upper, base, sizeof(base_upper));
    ToLowerStr(ext_lower, ext, sizeof(ext_lower));
    ToUpperStr(ext_upper, ext, sizeof(ext_upper));

    // If datadir is already determined, search datadir first
    if (datadir[0] != '\0')
    {
        const char *candidates[4][2] = {
            { base_lower, ext_lower },
            { base_upper, ext_upper },
            { base_lower, ext_upper },
            { base_upper, ext_lower }
        };
        for (int c = 0; c < 4; c++)
        {
            snprintf(outpath, outsize, "%s%s%s", datadir, candidates[c][0], candidates[c][1]);
            struct stat st;
            if (stat(outpath, &st) == 0)
                return true;
        }
    }

    // Search standard directories
    for (int d = 0; search_dirs[d] != NULL; d++)
    {
        const char *dir = search_dirs[d];
        const char *candidates[4][2] = {
            { base_lower, ext_lower },
            { base_upper, ext_upper },
            { base_lower, ext_upper },
            { base_upper, ext_lower }
        };
        for (int c = 0; c < 4; c++)
        {
            snprintf(outpath, outsize, "%s%s%s", dir, candidates[c][0], candidates[c][1]);
            struct stat st;
            if (stat(outpath, &st) == 0)
            {
                // If datadir not yet set and this isn't romfs, record datadir
                if (datadir[0] == '\0' && strncmp(dir, "romfs:", 6) != 0)
                {
                    strncpy(datadir, dir, sizeof(datadir) - 1);
                }
                return true;
            }
        }
    }

    return false;
}

int Switch_OpenGameFile(const char *base, const char *ext, int flags)
{
    char fullpath[512];
    if (Switch_FindGameFile(fullpath, sizeof(fullpath), base, ext))
    {
        return open(fullpath, flags);
    }
    char simplepath[64];
    snprintf(simplepath, sizeof(simplepath), "%s%s", base, ext);
    return open(simplepath, flags);
}

#endif
