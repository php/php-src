--TEST--
A locked class prevents defined properties being unset
--FILE--
<?php

locked class TestClass {
	public $definedProp;
}

$t = new testClass();

$t->definedProp = "OK\n";
unset($t->definedProp);

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--

Fatal error: Uncaught Error: Cannot unset property %s of locked class %s in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
