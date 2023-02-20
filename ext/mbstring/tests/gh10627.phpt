--TEST--
GH-10627 (mb_convert_encoding crashes PHP on Windows)
--EXTENSIONS--
mbstring
--FILE--
<?php

$str = 'Sökinställningar';
$data = [$str, 'abc'];
var_dump(mb_convert_encoding($data, 'UTF-8', 'auto'));
$data = [$str => 'abc', 'abc' => 'def'];
var_dump(mb_convert_encoding($data, 'UTF-8', 'auto'));

?>
--EXPECT--
array(2) {
  [0]=>
  string(16) "S?kinst?llningar"
  [1]=>
  string(3) "abc"
}
array(2) {
  ["S?kinst?llningar"]=>
  string(3) "abc"
  ["abc"]=>
  string(3) "def"
}

