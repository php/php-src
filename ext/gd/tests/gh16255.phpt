--TEST--
GH-16255 (Unexpected nan value in ext/gd/libgd/gd_filter.c)
--EXTENSIONS--
gd
--CREDITS--
cmb69
--FILE--
<?php
$matrix = array(array(1, 0, 1), array(0, 5, 0), array(1, 0, 1));
$im = imagecreatetruecolor(40, 40);

try {
    imageconvolution($im, $matrix, NAN, 1.0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    imageconvolution($im, $matrix,  2.225E-307, 1.0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    imageconvolution($im, $matrix, 1, NAN);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
imageconvolution(): Argument #3 ($divisor) must be finite
imageconvolution(): Argument #3 ($divisor) must not be 0
imageconvolution(): Argument #4 ($offset) must be finite
