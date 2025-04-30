--TEST--
GH-14387 (Crash when stack walking in destructor of yielded from values during Generator->throw())
--FILE--
<?php

function prime(Generator $generator) {
	$generator->valid();
}

$g = (function () {
	yield from [null, new class {
		function __destruct() {
			// Trigger a stack walk, hitting a bad frame.
			throw new Exception;
		}
	}];
})();

prime($g);

$g->throw(new Error);

?>
--EXPECTF--
Fatal error: Uncaught Error in %s:%d
Stack trace:
#0 {main}

Next Exception in %s:%d
Stack trace:
#0 %s(%d): class@anonymous->__destruct()
#1 [internal function]: {%s}()
#2 %s(%d): Generator->throw(Object(Error))
#3 {main}
  thrown in %s on line %d
