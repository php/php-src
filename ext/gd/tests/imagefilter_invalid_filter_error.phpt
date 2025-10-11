--TEST--
GH-20070: Testing wrong parameter passing in imagefilter() of GD library
--EXTENSIONS--
gd
--FILE--
<?php
$image = imagecreatetruecolor(1, 1);

try {
    var_dump(imagefilter($image, -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: imagefilter(): Argument #2 ($filter) must be one of the IMG_FILTER_* filter constants
