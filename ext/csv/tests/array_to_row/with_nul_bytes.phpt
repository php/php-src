--TEST--
Test Csv\array_to_row() with nul bytes
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$fields = [
    'Hello',
    "Field has\0spaces",
    "Has delimiter,\0inside field",
    "Has field escape sequence\0\" inside field",
    'Basic',
];

$output = "Hello,Field has\0spaces,\"Has delimiter,\0inside field\",\"Has field escape sequence\0\"\" inside field\",Basic\r\n";

var_dump($output === Csv\array_to_row($fields));
var_dump($output === Csv\array_to_row($fields, ','));
var_dump($output === Csv\array_to_row($fields, ',', '"'));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
