# XVolkolak

XVolkolak is a Qt/CMake emulator-unpacker project with two front ends:

- `xvolkolak` - Qt Widgets GUI application.
- `xvolkolakc` - console application.

The core (`_mylibs/XUniversalUnpacker/emulatorunpacker.*`) drives the **XEmulUnpacker** engine
(`_mylibs/XEmulUnpacker`, built on the XEmulator user-mode CPU emulator): it loads a
packed executable, single-steps the loader stub until the transfer to the original
entry point (OEP), and writes the reconstructed image out. The packer can be left on
`Generic (auto)` (heuristic OEP detection) or set to one of 21 packer-specific
unpackers (UPX, ASPack, NSPack, (Win)Upack, FSG, MEW, MPRESS, PECompact, ACProtect,
!EP(EXE Pack), PeX, AHPacker, BeRoEXEPacker, ExeFog, nPack, Fish PE Packer, kkrunchy,
Packman, QuickPack NT, Petite, REVProt). Run `xvolkolakc --list-packers` to enumerate.

## Build

```bat
cmake -S . -B tmp_build -DCMAKE_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64
cmake --build tmp_build --config Release
```

For Visual Studio generator builds, pass `-G "Visual Studio 17 2022" -A x64`
or the platform you need.

## Console Usage

```bat
xvolkolakc input.exe --result-directory C:\unpacked
```

The result directory defaults to the input file's directory. Output names are
derived automatically from the original name and unpack method. For example,
`sample.tar.gz` unpacked with `UPX signature` is written as:

```text
C:\unpacked\sample.tar.gz.unpacked\sample.unpacked[UPX signature].tar.gz
```

Use `--force` to overwrite an existing derived output file.
