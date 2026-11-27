--TEST--
imageaffine(): the TypeError for an invalid element names the $affine argument
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(20, 20);
try {
    imageaffine($im, [1, 0, null, 1, 0, 0]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: imageaffine(): Argument #2 ($affine) contains invalid type for element 2
