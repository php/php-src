--TEST--
Test Csv\array_to_row() with multi byte delimiter
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
    'Has delimiter。 inside field',
    'Has field escape sequence " inside field',
    'Basic',
];

$output = "Hello。Field has spaces。\"Has delimiter。 inside field\"。\"Has field escape sequence \"\" inside field\"。Basic\r\n";
var_dump($output === Csv\array_to_row($fields, '。', '"'));
var_dump(Csv\array_to_row($fields, '。', '"'));

?>
--EXPECT--
bool(true)
string(114) "Hello。Field has spaces。"Has delimiter。 inside field"。"Has field escape sequence "" inside field"。Basic
"
