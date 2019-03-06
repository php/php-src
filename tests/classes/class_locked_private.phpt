--TEST--
A locked class cannot set undefined properties on its own instances
--FILE--
<?php

locked class TestClass {
	private $definedProp;
	
	public function test() {
		$this->definedProp = "OK\n";
		echo $this->definedProp;

		$this->nonExistentProp = "Not OK\n";
	}
}

$t = new testClass();
$t->test();

echo "Done\n"; // shouldn't be displayed
?>
--EXPECTF--
OK

Fatal error: Uncaught Error: Cannot write undefined property %s on locked class %s in %s:%d
Stack trace:
#0 %s(%d): TestClass->test()
#1 {main}
  thrown in %s on line %d
