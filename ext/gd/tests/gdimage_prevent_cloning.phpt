--TEST--
Checks that GdImage instances cannot be cloned
--EXTENSIONS--
gd
--FILE--
<?php

try {
    $img_src = imagecreatetruecolor(32, 32);
    $img_dst = clone $img_src;
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class GdImage
