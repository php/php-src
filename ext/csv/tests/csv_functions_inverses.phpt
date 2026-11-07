--TEST--
Test cvs functions to see if they can invert them self
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
    'Has field escape sequence " inside field',
    'Basic',
];
var_dump($fields === Csv\row_to_array(Csv\array_to_row($fields)));

?>
--EXPECT--
bool(true)
