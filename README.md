# GeekCampVol6
## 開発用のコマンド
初回ビルド
```
cmake -S . -B build -G Ninja 
cmake --build build
.\build\SDLGame.exe
```
二回目以降
```
cmake --build build
.\build\SDLGame.exe
```
SDL2とgccの環境は各自インストールしてください。
