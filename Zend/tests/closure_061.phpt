--TEST--
Closure::call() or Closure::bind() to independent class
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
string(3) "foo"
