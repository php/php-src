--TEST--
Test various types passed to mixed type.
--FILE--
<?php

$types = [
	NULL,
	TRUE,
	123,
	123.456,
	'php',
	new stdClass,
	fopen(__FILE__, 'r'),
];

function test(mixed $arg) {
	var_dump($arg);
}

foreach ($types as $type) {
	test($type);
}
--EXPECTF--
NULL
bool(true)
int(123)
float(123.456)
string(3) "php"
object(stdClass)#%d (%d) {
}
resource(%d) of type (stream)
