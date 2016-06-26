--TEST--
var_info() correctly identifies numeric strings
--FILE--
<?php

$data = [
	'1',
	"1",
	'1.0',
	"1.0",
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'numeric string' || var_dump($type);
}

?>
--EXPECT--
