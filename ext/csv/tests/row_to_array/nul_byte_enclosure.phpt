--TEST--
Test Csv\row_to_array() with enclosure as a nul byte
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
    'Field has spaces',
    'Has delimiter, inside field',
    "Has field escape sequence \0 inside field",
    'Basic',
];

$string = "Hello,Field has spaces,\0Has delimiter, inside field\0,\0Has field escape sequence \0\0 inside field\0,Basic\r\n";

var_dump($fields === Csv\row_to_array($string, ',', "\0"));

?>
--EXPECT--
bool(true)
