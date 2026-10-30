--TEST--
GH-10627 (mb_convert_encoding crashes PHP on Windows)
--EXTENSIONS--
mbstring
--INI--
mbstring.strict_detection=1
--FILE--
<?php

$str = "S\xF6kinst\xE4llningar";
$data = [$str, 'abc'];
var_dump(mb_convert_encoding($data, 'UTF-8', 'auto'));
$data = [$str => 'abc', 'abc' => 'def'];
var_dump(mb_convert_encoding($data, 'UTF-8', 'auto'));
$data = ['abc' => $str, 'def' => 'abc'];
var_dump(mb_convert_encoding($data, 'UTF-8', 'auto'));

?>
--EXPECTF--
Warning: mb_convert_encoding(): Unable to detect character encoding in %s on line %d
array(1) {
  [1]=>
  string(3) "abc"
}

Warning: mb_convert_encoding(): Unable to detect character encoding in %s on line %d
array(1) {
  ["abc"]=>
  string(3) "def"
}

Warning: mb_convert_encoding(): Unable to detect character encoding in %s on line %d
array(1) {
  ["def"]=>
  string(3) "abc"
}
