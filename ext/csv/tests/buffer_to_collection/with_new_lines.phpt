--TEST--
Test Csv\buffer_to_collection() with standard parameters
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$collection = [
    [
        'One',
        "I have a\n new line",
        'Three',
    ],
    [
        'Four',
        "I have a\r carriage return",
        'Six',
    ],
    [
        'Seven',
        "I have a carriage\r\n return new line",
        'Nine',
    ],
];

$string = "One,\"I have a\n new line\",Three\r\nFour,\"I have a\r carriage return\",Six\r\nSeven,\"I have a carriage\r\n return new line\",Nine\r\n";

var_dump($collection === Csv\buffer_to_collection($string));

?>
--EXPECT--
bool(true)
