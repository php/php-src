--TEST--
GH-20551: (imagegammacorrect out of range input/output value)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreate(64, 32);

try {
	imagegammacorrect($im, NAN, 1.0);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	imagegammacorrect($im, -NAN, 1.0);
} catch (\ValueError $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
An input divided by an output must be finite
An input divided by an output must be finite
