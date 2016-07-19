--TEST--
var_info() correctly identifies bool false
--FILE--
<?php

$data = [
	false,
	FALSE,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'false' || var_dump($type);
}

?>
--EXPECT--
