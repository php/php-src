--TEST--
Testing exception type executor with return pass
--FILE--
<?php
class FancyException extends Exception {}

function test () : int throws FancyException {
	throw new FancyException();
}

try {
	test();
} catch (FancyException $ex) {}
?>
OK
--EXPECT--
OK
