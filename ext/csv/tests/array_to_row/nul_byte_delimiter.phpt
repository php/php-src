--TEST--
Test Csv\array_to_row() with delimiter as a nul byte
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
    "Has delimiter\0 inside field",
    'Has field escape sequence " inside field',
    'Basic',
];

$output = "Hello\0Field has spaces\0\"Has delimiter\0 inside field\"\0\"Has field escape sequence \"\" inside field\"\0Basic\r\n";

var_dump($output === Csv\array_to_row($fields, "\0"));
var_dump($output === Csv\array_to_row($fields, "\0", '"'));

?>
--EXPECT--
bool(true)
bool(true)
