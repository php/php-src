--TEST--
Testing each() with array and object
--FILE--
<?php

$a = new stdClass;
$foo = each($a);
var_dump($foo);

$a = new stdClass;
var_dump(each($a));

$a = array(new stdClass);
var_dump(each($a));


?>
--EXPECTF--
Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
bool(false)
bool(false)
array(4) {
  [1]=>
  object(stdClass)#1 (0) {
  }
  ["value"]=>
  object(stdClass)#1 (0) {
  }
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}
