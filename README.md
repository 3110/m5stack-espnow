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
