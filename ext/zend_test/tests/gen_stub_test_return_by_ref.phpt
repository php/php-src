--TEST--
gen_stub.php: Test that return by ref flag is set
--XFAIL--
Does not work currently
--EXTENSIONS--
zend_test
--FILE--
<?php

$reflectionMethod = new ReflectionMethod(_ZendTestClass::class, "returnByRefIntProp");
var_dump($reflectionMethod->returnsReference());

$o = new _ZendTestClass();
var_dump($o);
$i = $o->returnByRefIntProp();
var_dump($i);
$i = 24;
var_dump($i);
var_dump($o);

?>
--EXPECT--
bool(true)
object(_ZendTestClass)#2 (3) {
  ["intProp"]=>
  int(123)
  ["classProp"]=>
  NULL
  ["classUnionProp"]=>
  NULL
  ["classIntersectionProp"]=>
  uninitialized(Traversable&Countable)
  ["readonlyProp"]=>
  uninitialized(int)
  ["dnfProperty"]=>
  uninitialized(Iterator|(Traversable&Countable))
}
int(123)
int(24)
object(_ZendTestClass)#2 (3) {
  ["intProp"]=>
  int(24)
  ["classProp"]=>
  NULL
  ["classUnionProp"]=>
  NULL
  ["classIntersectionProp"]=>
  uninitialized(Traversable&Countable)
  ["readonlyProp"]=>
  uninitialized(int)
  ["dnfProperty"]=>
  uninitialized(Iterator|(Traversable&Countable))
}
