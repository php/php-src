--TEST--
A locked class prevents undefined properties being accessed
--FILE--
<?php

locked class TestClass {
	public $definedProp;
}

$t = new testClass();

$t->definedProp = "OK\n";
echo $t->definedProp;

echo $t->nonExistentProp;

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
OK

Fatal error: Uncaught Error: Cannot access undefined property %s on locked class %s in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
