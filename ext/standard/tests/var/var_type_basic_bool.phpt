--TEST--
var_type() correctly identifies bool variables
--FILE--
<?php

$data = [
	false,
	FALSE,
	true,
	TRUE,
];

foreach ($data as $datum) {
	$type = var_type($datum);

	$type === 'bool' || var_dump($type);
}

?>
--EXPECT--
