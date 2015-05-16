--TEST--
Throwing exception using a class that isn't derived from the Exception base class
--FILE--
<?php 

error_reporting(E_ALL|E_STRICT);

class Foo { }

try {
	throw new Foo();
} catch (Foo $e) {
	var_dump($e);
}

?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Exceptions must be valid objects derived from the Exception base class' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
