# Chibicraft
## 概要
Chibicraftは「できるだけフルスクラッチでMinecraftを再現する」ことを目標として作られたゲームである。
現在の機能としては、主にワールドの探索、ブロックの配置、破壊を行うことができる。

![Chibicraft](chibicraft.jpg)

## 環境構築
Chibicraftは現在、Ubuntu22.04上で正常に動作することが確認されている。
また、Kali Linux2024.1上では、動作はするが視点操作が反転するというバグが発見されている。
Mac OSやWindows上での動作は保証されていないため、Virtual Machine等を用いてUbuntu上で動かすことを推奨している。

## Ubuntu上での環境構築手順
まず、SDL1.2とOpenGL Mathematicsをインストールする。
```bash
$ sudo apt update
$ sudo apt install libsdl1.2-dev -y
$ sudo apt install libglm-dev -y
```
次にChibicraftをインストールする。
```bash
$ git clone --recursive https://github.com/matajupi/chibigit.git
$ cd Chibicraft
$ make
```
ChibicraftはChibicraftディレクトリ直下で次のコマンドを実行することにより起動することができる。
```bash
$ ./bin/chibi
```

## ゲームの操作
    | キー            | 説明                                  |
    | --------------- | ------------------------------------- |
    | w               | 前進                                  |
    | s               | 後退                                  |
    | a               | 左に移動                              |
    | d               | 右に移動                              |
    | マウスカーソル  | 視点操作                              |
    | 左クリック      | ブロックを破壊                        |
    | 右クリック      | ブロックを配置                        |
    | 左矢印          | ブロックの変更(種類は画面左上に表示)  |
    | 右矢印          | ブロックの変更(種類は画面左上に表示)  |

## アピールしたい点
ChibicraftはC++で書かれており、ゲームエンジンにQuickCGを利用している。
ChibicraftはできるだけフルスクラッチでMinecraftを再現することを目標としているため、3D描画に特化したゲームエンジンやライブラリ等は利用していない。
また、LLMも利用していない。
3Dの描画にはRaycastingというシンプルなアルゴリズムを用いている。
また、Raycastingの高速化のためにOpenMPによる並列化を行っている。

