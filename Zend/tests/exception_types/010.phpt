--TEST--
Testing exception type executor fail
--DESCRIPTION--
I have no idea how to update Optimizer, so temporarily disabled it
--INI--
opcache.optimization_level=0
--FILE--
<?php
function test () throws FancyException {
	throw new Exception();
}
test();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Exception thrown by test() expected to be an instance of FancyException, instance of Exception thrown in %s:3
Stack trace:
#0 %s(5): test()
#1 {main}
  thrown in %s on line 3

