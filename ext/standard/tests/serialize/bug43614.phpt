--TEST--
Bug #43614 (incorrect processing of numerical string keys of array in arbitrary serialized data)
--FILE--
<?php

error_reporting(E_ALL);

var_dump($a = unserialize('a:2:{s:2:"10";i:1;s:2:"01";i:2;}'));
var_dump($a['10']);
var_dump($a[b'01']);

?>
--EXPECT--
array(2) {
  [10]=>
  int(1)
  ["01"]=>
  int(2)
}
int(1)
int(2)
