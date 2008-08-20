--TEST--
Bug #42737 (preg_split('//u') triggers a E_NOTICE with newlines)
--FILE--
<?php

$string = chr(13).chr(10);

$array = preg_split('//u', $string, - 1, PREG_SPLIT_NO_EMPTY);

var_dump(array_map('ord', $array));

?>
--EXPECT--
array(2) {
  [0]=>
  int(13)
  [1]=>
  int(10)
}
