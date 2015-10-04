--TEST--
Closure::call() or Closure::bind() to independent class must fail
--FILE--
<?php

class foo {
	public $var;
    
	function initClass() {
		$this->var = __CLASS__;
	}
}

class bar {
	public $var;
    
	function initClass() {
		$this->var = __CLASS__;
	}
    
	function getVar() {
		assert($this->var !== null); // ensure initClass was called
		return $this->var;
	}
}

class baz extends bar {
	public $var;
    
	function initClass() {
		$this->var = __CLASS__;
	}
}

function callMethodOn($class, $method, $object) {
	$closure = (new ReflectionMethod($class, $method))->getClosure((new ReflectionClass($class))->newInstanceWithoutConstructor());
	$closure = $closure->bindTo($object, $class);
	return $closure();
}

$baz = new baz;

callMethodOn("baz", "initClass", $baz);
var_dump($baz->getVar());

callMethodOn("bar", "initClass", $baz);
var_dump($baz->getVar());

callMethodOn("foo", "initClass", $baz);
var_dump($baz->getVar());

?>
--EXPECTF--
string(3) "baz"
string(3) "bar"

Warning: Cannot bind function foo::initClass to object of class baz in %s on line %d

Fatal error: Uncaught Error: Function name must be a string in %s:%d
Stack trace:
#0 %s(%d): callMethodOn('foo', 'initClass', Object(baz))
#1 {main}
  thrown in %s on line %d
