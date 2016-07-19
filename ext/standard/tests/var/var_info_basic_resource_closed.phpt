--TEST--
var_info() correctly identifies closed resources
--FILE--
<?php

$closed = tmpfile();
fclose($closed);

$data = [
	$closed,
];

foreach ($data as $datum) {
	$type = var_info($datum);

	$type === 'closed resource' || var_dump($type);
}

?>
--EXPECT--
