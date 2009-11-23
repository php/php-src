--TEST--
Bug #50261 (Crash When Calling Parent Constructor with call_user_func())
--FILE--
<?php

class testClass {
	function testClass($x) {
		echo __METHOD__, " (". $x . ")\n";
	}
}

class testClass2 extends testClass {
	function __construct() {
		static $x = 0;
		
		if ($x) {
			print "Infinite loop...\n";
		} else {
			$x++;
			
			parent::__construct(1);
			testclass::__construct(2);
			call_user_func(array('parent', '__construct'), 3);
			call_user_func(array('testclass', '__construct'), 4);
			call_user_func(array('testclass', 'testclass'), 5);
		}
	}
}

new testClass2;

?>
--EXPECT--
testClass::testClass (1)
testClass::testClass (2)
testClass::testClass (3)
testClass::testClass (4)
testClass::testClass (5)
