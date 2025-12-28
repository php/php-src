--TEST--
GH-16322 (imageaffine overflow/underflow on affine matrix)
--EXTENSIONS--
gd
--INI--
memory_limit=-1
--FILE--
<?php
$matrix = [INF, 1, 1, 1, 1, 1];
$src = imagecreatetruecolor(8, 8);

try {
	imageaffine($src, $matrix);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
$matrix[0] = 1;
$matrix[3] = -INF;
try {
	imageaffine($src, $matrix);
} catch (\ValueError $e) {
	echo $e->getMessage();
}
?>
--EXPECTF--
imageaffine(): Argument #2 ($affine) element 0 must be between %s and %d
imageaffine(): Argument #2 ($affine) element 3 must be between %s and %d
