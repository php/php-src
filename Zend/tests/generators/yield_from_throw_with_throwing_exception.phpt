--TEST--
Exceptions are properly appended when thrown from yield from values destruction
--FILE--
<?php

function gen() {
	yield from [1, 2, new class {
		function __destruct() {
			throw new Exception("dtor");
		}
	}];
}

gen()->throw(new Exception("outer"));

?>
--EXPECTF--
Fatal error: Uncaught Exception: outer in %s:%d
Stack trace:
#0 {main}

Next Exception: dtor in %s:%d
Stack trace:
#0 %s(%d): class@anonymous->__destruct()
#1 [internal function]: gen()
#2 %s(%d): Generator->throw(Object(Exception))
#3 {main}
  thrown %s on line %d
