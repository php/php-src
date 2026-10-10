--TEST--
Test Csv\row_to_array() with delimiter as a nul byte
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
    "Has delimiter, inside field",
    "Has custom EOL sequence \0 inside field",
    'Basic',
];

$string = "Hello,Field has spaces,\"Has delimiter, inside field\",\"Has custom EOL sequence \0 inside field\",Basic\0";

var_dump($fields === Csv\row_to_array($string, ',', '"', "\0"));

?>
--EXPECT--
bool(true)
