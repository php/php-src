--TEST--
__PHP_Incomplete_Class cannot be cloned
--FILE--
<?php
$o = unserialize('O:3:"Foo":0:{}');
var_dump($o::class);

$rc = new ReflectionClass("__PHP_Incomplete_Class");
var_dump($rc->isCloneable());
var_dump(clone($o));
?>
--EXPECTF--
string(22) "__PHP_Incomplete_Class"
bool(false)

Fatal error: Uncaught Error: Trying to clone an uncloneable object of class __PHP_Incomplete_Class in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
