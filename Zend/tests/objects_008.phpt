--TEST--
method overloading with different method signature
--INI--
error_reporting=8191
--FILE--
<?php

class test {
	function foo(Test $arg) {}
}

class test2 extends test {
	function foo(Test $arg) {}
}

class test3 extends test {
	function foo(Test3 $arg) {}
}

echo "Done\n";
?>
--EXPECTF--
Warning: Declaration of test3::foo(Test3 $arg) should be compatible with test::foo(Test $arg) in %s on line %d
Done
