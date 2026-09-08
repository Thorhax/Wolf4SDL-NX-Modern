# Wolf4SDL-Switch (Modern devkitPro / SDL2 Edition)

Port of Wolfenstein 3D to the Nintendo Switch based on Wolf4SDL, updated for modern **devkitPro** (devkitA64 / libnx 4.x) and **SDL2**.

## Features & Improvements
- **Modern devkitPro Toolchain**: Built using modern devkitA64 with C++17 and libnx 4.x `pad` API.
- **Upgraded to SDL2**: Migrated from legacy SDL 1.2 to official hardware-accelerated `switch-sdl2` and `switch-sdl2_mixer`.
- **Audio Popping & Crackling Fixed**: Replaced legacy OPL emulation with **Nuked OPL3**, delivering crystal-clear Sound Blaster / AdLib FM synthesis and sound effects.
- **Smooth Dual Analog Controls**: Full dual-analog stick support (Left stick: analog movement + strafe; Right stick: smooth turning with deadzone & curve).
- **Embedded RomFS Shareware**: Wolfenstein 3D Shareware Episode 1 is embedded in RomFS, allowing the port to run immediately out of the box without requiring manual file copying.
- **Full Version Auto-Detection**: Drop your full version game data (`.wl6`, `.wl3`, `.wl1`) into `/switch/wolf4sdl/` on your SD card and the engine will automatically detect and load it!
- **Case-Insensitive File Loading**: Supports both lowercase and uppercase DOS filenames.

## Controls

| Buttons / Sticks | In-Game Action | Menu Action |
| :--- | :--- | :--- |
| **Left Stick** | Analog Move (Y) & Strafe (X) | Navigate Menu Up / Down / Left / Right |
| **Right Stick** | Smooth Turning (X) | — |
| **D-Pad Up / Down** | Forward / Backward | Navigate Menu Up / Down |
| **D-Pad Left / Right** | Turn Left / Right | Change Option Left / Right |
| **A / ZR** | Fire / Attack | Confirm / Enter |
| **B** | Open Door / Elevator / Secret | Cancel / Back / Escape |
| **X** | Strafe Toggle | Answer "No" on prompts |
| **Y / ZL** | Run (Hold/Toggle) | Answer "Yes" on prompts |
| **L / R** | Previous / Next Weapon | — |
| **Left Stick Click** | Toggle Always Run | — |
| **Right Stick Click**| Cycle Weapon | — |
| **Minus (-)** | In-Game Menu / Escape | Return to Game / Escape |
| **Plus (+)** | Pause Game | Return to Game / Escape |

## Installation

1. Copy `Wolf4SDL.nro` to `/switch/wolf4sdl/Wolf4SDL.nro` on your Nintendo Switch SD card.
2. (Optional) If you own the full game, copy your Wolfenstein 3D data files (`*.wl6`) into `/switch/wolf4sdl/`:
   ```
   audiohed.wl6
   audiot.wl6
   gamemaps.wl6
   maphead.wl6
   vgadict.wl6
   vgagraph.wl6
   vgahead.wl6
   vswap.wl6
   ```
3. Launch **Wolfenstein 3D** from the Homebrew Menu.

## Compiling

Using the official devkitPro Docker container:

```bash
docker run --rm \
  -v $(pwd):/wolf3d \
  -w /wolf3d \
  devkitpro/devkita64:latest \
  bash -c "
    export PATH=/opt/devkitpro/devkitA64/bin:/opt/devkitpro/portlibs/switch/bin:/opt/devkitpro/tools/bin:\$PATH
    export PKG_CONFIG_PATH=/opt/devkitpro/portlibs/switch/lib/pkgconfig
    make -j\$(nproc)
  "
```

## Credits & Thanks
- **id Software** for the original Wolfenstein 3D
- **Moritz "Ripper" Kroll & Chaos-Software** for Wolf4SDL
- **Fabian Greffrath** for the modern SDL2 Wolf4SDL port & maintenance
- **Nukeykt** for the Nuked OPL3 emulator
- **Keeganator** for the original Nintendo Switch port
- **Thorhax** for the modern devkitPro / SDL2 / libnx 4.x update
- **devkitPro team** for the toolchains and Switch homebrew libraries
