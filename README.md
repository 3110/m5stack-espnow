# M5Stack用ESP-NOWライブラリ

M5Stack用のESP-NOWライブラリです。PlatformIO IDEで使用することを想定しています。

## インストール方法

`platformio.ini`に以下のように記述してください。

```ini
build_flags = -DESP_NOW_CHANNEL=0
lib_deps = https://github.com/3110/m5stack-espnow.git
```

## サンプルプログラム

`examples`ディレクトリにサンプルプログラムを用意しています。

このリポジトリはGit submoduleを使用しています。クローンする場合は、次のように `--recurse-submodules` を付けてください。

```sh
git clone --recurse-submodules https://github.com/3110/m5stack-espnow.git
```

すでに通常の `git clone` を実行した場合は、次のコマンドでsubmoduleを取得できます。

```sh
git submodule update --init --recursive
```
