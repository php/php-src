--TEST--
Test Csv\row_to_array() with new lines
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

$string = "\"I have a\n new line\",\"I have a\r carriage return\",\"I use a carriage\r\n return new line\",I don't have a new line\r\n";

var_dump($fields === Csv\row_to_array($string));
var_dump($fields === Csv\row_to_array($string, ','));
var_dump($fields === Csv\row_to_array($string, ',', '"'));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
