# Elf auf Zwölf

Translates D3D11 games to D3D12 using Microsoft's own
[D3D11On12](https://docs.microsoft.com/en-us/windows/desktop/direct3d12/direct3d-11-on-12)
wrapper library.

### Requirements

Windows 10 build 1809 is required for this to work properly. Might work with 1803, but that
was not tested.

### How to use

Just download the latest [release](https://github.com/doitsujin/elf-auf-zwoelf/releases),
copy the `d3d11.dll` next to the executable of the game you want to run, and start the game.
Make sure to use the `x64` one for 64-bit games, and the `x86 one for 32-bit games.

If everything works correctly, you will most likely experience reduced performance compared
to native D3D11, and in some cases rendering issues and/or driver crashes. Some games may
refuse to run entirely.

**Note:** Tools like MSI Afterburner will still show D3D11 in their overlay. This is because
they see a DXGI swap chain created for a D3D11 device, not the underlying D3D12 device.

### Why?

Why not? Just don't expect any sort of support when running into issues.
