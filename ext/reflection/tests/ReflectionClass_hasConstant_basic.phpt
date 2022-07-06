--TEST--
ReflectionClass::hasConstant()
--CREDITS--
Marc Veldman <marc@ibuildings.nl>
#testfest roosendaal on 2008-05-10
--FILE--
<?php
//New instance of class C - defined below
$rc = new ReflectionClass("C");

//Check if C has constant foo
var_dump($rc->hasConstant('foo'));

//C should not have constant bar
var_dump($rc->hasConstant('bar'));

Class C {
  const foo=1;
}
?>
--EXPECT--
bool(true)
bool(false)
