--TEST--
Test Csv\row_to_array() with standard parameters
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

$string = "Hello,Field has spaces,\"Has delimiter, inside field\",\"Has field escape sequence \"\" inside field\",Basic\r\n";

var_dump($fields === Csv\row_to_array($string));
var_dump(Csv\row_to_array($string));
var_dump(Csv\row_to_array($string, ','));
var_dump(Csv\row_to_array($string, ',', '"'));
var_dump(Csv\row_to_array($string, ',', '"', "\r\n"));

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
  string(40) "Has field escape sequence " inside field"
  [4]=>
  string(5) "Basic"
}
array(5) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(16) "Field has spaces"
  [2]=>
  string(27) "Has delimiter, inside field"
  [3]=>
  string(40) "Has field escape sequence " inside field"
  [4]=>
  string(5) "Basic"
}
array(5) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(16) "Field has spaces"
  [2]=>
  string(27) "Has delimiter, inside field"
  [3]=>
  string(40) "Has field escape sequence " inside field"
  [4]=>
  string(5) "Basic"
}
array(5) {
  [0]=>
  string(5) "Hello"
  [1]=>
  string(16) "Field has spaces"
  [2]=>
  string(27) "Has delimiter, inside field"
  [3]=>
  string(40) "Has field escape sequence " inside field"
  [4]=>
  string(5) "Basic"
}
