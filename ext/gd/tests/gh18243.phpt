--TEST--
GH-18243: imagefttext underflow/overflow on $size
--EXTENSIONS--
gd
--SKIPIF--
<?php
if(!function_exists('imagettftext')) die('skip imagettftext() not available');
?>
--FILE--
<?php
$font = __DIR__.'/Rochester-Regular.otf';
$im = imagecreatetruecolor(100, 80);

try {
	imagettftext($im, PHP_INT_MAX, 0, 15, 60, 0, $font, "");
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	imagettftext($im, PHP_INT_MIN, 0, 15, 60, 0, $font, "");
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	imagettftext($im, NAN, 0, 15, 60, 0, $font, "");
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	imagettftext($im, INF, 0, 15, 60, 0, $font, "");
} catch (\ValueError $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
ValueError: imagettftext(): Argument #2 ($size) must be between %i and %d
ValueError: imagettftext(): Argument #2 ($size) must be between %i and %d
ValueError: imagettftext(): Argument #2 ($size) must be finite
ValueError: imagettftext(): Argument #2 ($size) must be between %i and %d
