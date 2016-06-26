--TEST--
var_info() correctly identifies bool true
--FILE--
<?php

$data = [
	true,
	TRUE,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'true' || var_dump($type);
}

?>
--EXPECT--
