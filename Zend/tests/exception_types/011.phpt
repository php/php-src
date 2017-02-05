--TEST--
Testing exception type executor pass
--FILE--
<?php
class FancyException extends Exception {}

function test () throws FancyException {
	throw new FancyException();
}

try {
	test();
} catch (FancyException $ex) {}
?>
OK
--EXPECT--
OK
