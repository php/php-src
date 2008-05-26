--TEST--
Testing each() with recursion
--FILE--
<?php

$a = array(array());
$a[] =& $a;

var_dump(each($a[1]));

?>
--EXPECT--
array(4) {
  [1]=>
  array(0) {
  }
  [u"value"]=>
  array(0) {
  }
  [0]=>
  int(0)
  [u"key"]=>
  int(0)
}
