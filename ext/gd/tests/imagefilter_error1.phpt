--TEST--
Testing wrong parameter passing in imagefilter() of GD library
--CREDITS--
Guilherme Blanco <guilhermeblanco [at] hotmail [dot] com>
#testfest PHPSP on 2009-06-20
--EXTENSIONS--
gd
--FILE--
<?php
$image = imagecreatetruecolor(180, 30);

try {
    var_dump(imagefilter($image));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(imagefilter(20, 1));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
imagefilter() expects at least 2 arguments, 1 given
imagefilter(): Argument #1 ($image) must be of type GdImage, int given
