--TEST--
GH-16260 (Overflow/underflow on imagerotate degrees argument)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreatetruecolor(10,10);

try {
	imagerotate($im, PHP_INT_MIN, 0);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}

try {
	imagerotate($im, PHP_INT_MAX, 0);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
--EXPECTF--
imagerotate(): Argument #2 ($angle) must be between %s and %s
imagerotate(): Argument #2 ($angle) must be between %s and %s
