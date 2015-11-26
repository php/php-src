--TEST--
Testing each() with recursion
--INI--
zend.enable_gc=1
--FILE--
<?php

$a = array(array());
$a[] =& $a;

var_dump(each($a[1]));

?>
--EXPECTF--
array(4) {
  [1]=>
  array(0) {
  }
  ["value"]=>
  array(0) {
  }
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}
