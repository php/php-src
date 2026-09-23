--TEST--
Test Csv\row_to_array() with multi byte enclosure
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

$csv_line = "Hello。Field has spaces。あHas delimiter。 inside fieldあ。あHas field escape sequence ああ inside fieldあ。Basic\r\n";
var_dump($fields === Csv\row_to_array($csv_line, '。', 'あ'));
var_dump(Csv\row_to_array($csv_line, '。', 'あ'));

?>
--EXPECT--
bool(true)
array(5) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(16) "Field has spaces"
  [2]=>
  string(29) "Has delimiter。 inside field"
  [3]=>
  string(42) "Has field escape sequence あ inside field"
  [4]=>
  string(5) "Basic"
}
