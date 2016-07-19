--TEST--
var_info() correctly identifies strings
--FILE--
<?php

$data = [
	'',
	"",
	'string',
	"string",
	'10abc',
	"10abc",
	'abc10',
	"abc10",
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'string' || var_dump($type);
}

?>
--EXPECT--
