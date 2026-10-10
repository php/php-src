--TEST--
Test Csv\array_to_row() with multi byte enclosure
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
    'Has field escape sequence あ inside field',
    'Basic',
];

$output = "Hello。Field has spaces。あHas delimiter。 inside fieldあ。あHas field escape sequence ああ inside fieldあ。Basic\r\n";
var_dump($output === Csv\array_to_row($fields, '。', 'あ'));
var_dump(Csv\array_to_row($fields, '。', 'あ'));

?>
--EXPECT--
bool(true)
string(126) "Hello。Field has spaces。あHas delimiter。 inside fieldあ。あHas field escape sequence ああ inside fieldあ。Basic
"
