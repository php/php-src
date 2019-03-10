--TEST--
A locked class with __unset overload will only error for public properties
--FILE--
<?php

locked class TestClass {
	public $definedProp;
	private $privateProp;

	public function __unset($prop) {
		echo "__unset called for $prop\n";
	}
}

$t = new testClass();

unset($t->nonExistentProp);
unset($t->privateProp);

$t->definedProp = "OK\n";
unset($t->definedProp);

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
__unset called for nonExistentProp
__unset called for privateProp

Fatal error: Uncaught Error: Cannot unset property %s of locked class %s in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
