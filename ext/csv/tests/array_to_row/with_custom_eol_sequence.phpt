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
    'Has delimiter, inside field',
    'Has custom EOL sequence あ inside field',
    'Basic',
];

$output = 'Hello,Field has spaces,"Has delimiter, inside field","Has custom EOL sequence あ inside field",Basicあ';
var_dump($output === Csv\array_to_row($fields, ',', '"', 'あ'));
var_dump(Csv\array_to_row($fields, ',', '"', 'あ'));

?>
--EXPECT--
bool(true)
string(104) "Hello,Field has spaces,"Has delimiter, inside field","Has custom EOL sequence あ inside field",Basicあ"
