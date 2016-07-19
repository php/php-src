--TEST--
var_info() correctly identifies null
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
	$type = var_info($datum);

	$type === 'null' || var_dump($type);
}

$type = var_info($undefined);

$type === 'null' || var_dump($type);

?>
--EXPECTF--

Notice: Undefined variable: unset in %s on line %d

Notice: Undefined variable: undefined in %s on line %d
