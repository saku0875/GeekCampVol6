# GeekCampVol6
## 開発用のコマンド
初回ビルド
```
cmake -S . -B build/sdl3 -G Ninja -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe
cmake --build build/sdl3
.\build\sdl3\SDLGame.exe
```
二回目以降
```
cmake --build build/sdl3
.\build\sdl3\SDLGame.exe
```
SDL3とgccの環境は各自インストールしてください。
