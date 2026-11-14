--TEST--
Test Csv\row_to_array() with new lines
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

$string = 'Hello,Field has spaces,"Has delimiter, inside field","Has custom EOL sequence あ inside field",Basicあ';

var_dump($fields === Csv\row_to_array($string, ',', '"', 'あ'));
var_dump(Csv\row_to_array($string, ',', '"', 'あ'));

?>
--EXPECT--
bool(true)
array(5) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(16) "Field has spaces"
  [2]=>
  string(27) "Has delimiter, inside field"
  [3]=>
  string(40) "Has custom EOL sequence あ inside field"
  [4]=>
  string(5) "Basic"
}
