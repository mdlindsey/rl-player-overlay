# BakkesMod Plugin Template

This template uses `vcpkg` in an effort to decouple external dependencies from source code.

## Getting Started

Run the command below on initial setup.

```
.\Setup.ps1
```

If you're using Visual Studio you can trigger builds with `Ctrl + B`.

If you're building from command line, you can use `msbuild` in the root of the repo.

```
msbuild BakkesPlugin.sln
```

## Renaming Placeholders

It is recommended to use your own project/plugin name instead of the `BakkesPlugin` placeholder due to the fact that BakkesMod Console (F6) prefixes logs with class name; eg: `[class BakkesPlugin] RL window is focused`

To do this, find and replace all instances of `BakkesPlugin` in both source code and file/directory names.

Affected files/directories:

| File/Directory           | Rename | Source |
|--------------------------|:------:|:------:|
| `./BakkesPlugin`         |   ✅   |        |
| `./BakkesPlugin.sln`     |   ✅   |   ✅  |
| `./BakkesPlugin.vcxproj` |   ✅   |   ✅  |
| `./Setup.ps1`            |        |    ✅  |

To change DLL filename update `<TargetName>` prop in `BakkesPlugin.vcxproj`.
