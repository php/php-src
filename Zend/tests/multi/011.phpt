--TEST--
basic union exception test
--FILE--
<?php
class Foo {}

$foo = function($throw) : Foo | void {
	if($throw) {
		throw new Exception();
	}
	return new Foo();
};

var_dump($foo(0));
var_dump($foo(1));
?>
--EXPECTF--
object(Foo)#2 (0) {
}

Fatal error: Uncaught Exception in %s:6
Stack trace:
#0 %s(12): {closure}(1)
#1 {main}
  thrown in %s on line 6
