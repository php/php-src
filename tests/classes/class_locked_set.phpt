--TEST--
A locked class prevents undefined properties being set
--FILE--
<?php

locked class TestClass {
	public $definedProp;
}

$t = new testClass();

$t->definedProp = "OK\n";
echo $t->definedProp;

$t->nonExistentProp = "Not OK\n";

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
OK

Fatal error: Uncaught Error: Cannot write undefined property %s on locked class %s in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
