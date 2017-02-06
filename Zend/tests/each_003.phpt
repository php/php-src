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
Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
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
