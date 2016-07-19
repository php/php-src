--TEST--
var_type() correctly identifies resource variables
--FILE--
<?php

$closed = tmpfile();
fclose($closed);

$data = [
	STDIN,
	fopen(__FILE__, 'r'),
	opendir(__DIR__),
	$closed,
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'resource' || var_dump($type);
}

?>
--EXPECT--
