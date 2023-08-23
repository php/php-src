@for %%i in (16,32,64,128,256) do magick convert -size %%ix%%i php.svg php-ico-%%i.png

@magick convert -adjoin php-ico-* php.ico

@del /f /q php-ico-*.png
