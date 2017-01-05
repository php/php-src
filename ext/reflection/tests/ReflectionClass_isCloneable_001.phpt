--TEST--
Testing ReflectionClass::isCloneable()
--SKIPIF--
<?php if (!extension_loaded('simplexml') || !extension_loaded('xmlwriter')) die("skip SimpleXML and XMLWriter is required for this test"); ?>
--FILE--
<?php

class foo {
}
$foo = new foo;

print "User class\n";
$obj = new ReflectionClass($foo);
var_dump($obj->isCloneable());
$obj = new ReflectionObject($foo);
var_dump($obj->isCloneable());
$h = clone $foo;

class bar {
	private function __clone() {
	}
}
$bar = new bar;
print "User class - private __clone\n";
$obj = new ReflectionClass($bar);
var_dump($obj->isCloneable());
$obj = new ReflectionObject($bar);
var_dump($obj->isCloneable());
$h = clone $foo;

print "Closure\n";
$closure = function () { };
$obj = new ReflectionClass($closure);
var_dump($obj->isCloneable());
$obj = new ReflectionObject($closure);
var_dump($obj->isCloneable());
$h = clone $closure;

print "Internal class - SimpleXMLElement\n";
$obj = new ReflectionClass('simplexmlelement');
var_dump($obj->isCloneable());
$obj = new ReflectionObject(new simplexmlelement('<test></test>'));
var_dump($obj->isCloneable());
$h = clone new simplexmlelement('<test></test>');

print "Internal class - XMLWriter\n";
$obj = new ReflectionClass('xmlwriter');
var_dump($obj->isCloneable());
$obj = new ReflectionObject(new XMLWriter);
var_dump($obj->isCloneable());
$h = clone new xmlwriter;

?>
--EXPECTF--
User class
bool(true)
bool(true)
User class - private __clone
bool(false)
bool(false)
Closure
bool(true)
bool(true)
Internal class - SimpleXMLElement
bool(true)
bool(true)
Internal class - XMLWriter
bool(false)
bool(false)

Fatal error: Uncaught Error: Trying to clone an uncloneable object of class XMLWriter in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
