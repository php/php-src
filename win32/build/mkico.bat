@magick convert -size 512x512 php.svg php.bmp

@for %%i in (16,24,32,48,64,128,256) do @magick convert -scale %%ix%%i php.bmp php-ico-%%i.png

@magick convert -adjoin php-ico-* php.ico

@del /f /q php-ico-*.png php.bmp
