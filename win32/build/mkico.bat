@echo off
magick php.svg -size 512x512 php.bmp

for %%i in (16,24,32,48,64,128,256) do magick php.bmp -scale %%ix%%i php-ico-%%i.png

magick -adjoin php-ico-* php.ico

del /f /q php-ico-*.png php.bmp
