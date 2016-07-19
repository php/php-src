--TEST--
var_type() correctly identifies null variables
--FILE--
<?php

$unset = 'some data';
unset($unset);

$data = [
	null,
	NULL,
	$unset,
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'null' || var_dump($type);
}

$type = var_type($undefined);

$type === 'null' || var_dump($type);

?>
--EXPECTF--

Notice: Undefined variable: unset in %s on line %d

Notice: Undefined variable: undefined in %s on line %d
