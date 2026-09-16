--TEST--
GH-19578: imagefilledellipse underflow on width argument
--EXTENSIONS--
gd
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
?>
--FILE--
<?php
$src = imagecreatetruecolor(255, 255);

try {
    imagefilledellipse($src, 0, 0, PHP_INT_MAX, 254, 0);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    imagefilledellipse($src, 0, 0, -16, 254, 0);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: imagefilledellipse(): Argument #4 ($width) must be between 0 and %d
ValueError: imagefilledellipse(): Argument #4 ($width) must be between 0 and %d
