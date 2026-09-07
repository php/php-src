--TEST--
GH-20551: (imagegammacorrect out of range input/output value)
--EXTENSIONS--
gd
--FILE--
<?php
$im = imagecreate(64, 32);

$gammas = [
	[NAN, 1.0],
	[-NAN, 1.0],
	[INF, 1.0],
	[-INF, 1.0],
	[1.0, NAN],
	[1.0, -NAN],
	[1.0, INF],
	[1.0, -INF],
];

foreach ($gammas as $gamma) {
	try {
		imagegammacorrect($im, $gamma[0], $gamma[1]);
	} catch (\ValueError $e) {
		echo $e::class, ': ', $e->getMessage(), PHP_EOL;
	}
}
?>
--EXPECT--
ValueError: imagegammacorrect(): Argument #2 ($input_gamma) must be finite
ValueError: imagegammacorrect(): Argument #2 ($input_gamma) must be finite
ValueError: imagegammacorrect(): Argument #2 ($input_gamma) must be finite
ValueError: imagegammacorrect(): Argument #2 ($input_gamma) must be greater than 0
ValueError: imagegammacorrect(): Argument #3 ($output_gamma) must be finite
ValueError: imagegammacorrect(): Argument #3 ($output_gamma) must be finite
ValueError: imagegammacorrect(): Argument #3 ($output_gamma) must be finite
ValueError: imagegammacorrect(): Argument #3 ($output_gamma) must be greater than 0
