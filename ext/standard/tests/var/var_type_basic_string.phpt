--TEST--
var_type() correctly identifies string variables
--FILE--
<?php

$data = [
	'',
	"",
	'string',
	"string",
	'1',
	"1",
	'1.0',
	"1.0",
	'10abc',
	"10abc",
	'abc10',
	"abc10",
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'string' || var_dump($type);
}

?>
--EXPECT--
