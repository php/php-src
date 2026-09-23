--TEST--
Test Csv\array_to_row() with custom EOL sequence as a nul byte
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
    "Has custom EOL sequence \0 inside field",
    'Basic',
];

$output = "Hello,Field has spaces,\"Has delimiter, inside field\",\"Has custom EOL sequence \0 inside field\",Basic\0";

var_dump($output === Csv\array_to_row($fields, ',', '"', "\0"));

?>
--EXPECT--
bool(true)
