--TEST--
var_info() correctly identifies resources
--FILE--
<?php

$data = [
	STDIN,
	fopen(__FILE__, 'r'),
	opendir(__DIR__),
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'resource of type stream' || var_dump($type);
}

?>
--EXPECT--
