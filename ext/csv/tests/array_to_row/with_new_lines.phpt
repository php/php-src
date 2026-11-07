--TEST--
Test Csv\array_to_row() with new lines
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$fields = [
    "I have a\n new line",
    "I have a\r carriage return",
    "I use a carriage\r\n return new line",
    "I don't have a new line",
];

$output = "\"I have a\n new line\",\"I have a\r carriage return\",\"I use a carriage\r\n return new line\",I don't have a new line\r\n";

var_dump($output === Csv\array_to_row($fields));
var_dump($output === Csv\array_to_row($fields, ','));
var_dump($output === Csv\array_to_row($fields, ',', '"'));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
