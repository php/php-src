--TEST--
Testing each() with array and object
--FILE--
<?php

$foo = each(new stdClass);
var_dump($foo);

var_dump(each(new stdClass));

$a = array(new stdClass);
var_dump(each($a));


?>
--EXPECT--
bool(false)
bool(false)
array(4) {
  [1]=>
  object(stdClass)#1 (0) {
  }
  [u"value"]=>
  object(stdClass)#1 (0) {
  }
  [0]=>
  int(0)
  [u"key"]=>
  int(0)
}
