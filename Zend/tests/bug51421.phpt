--TEST--
Bug #51421 (Abstract __construct constructor argument list not enforced)
--FILE--
<?php

class ExampleClass {}

abstract class TestInterface {
	abstract public function __construct(ExampleClass $var);
}

class Test extends TestInterface {
	public function __construct() {}
}

?>
--EXPECTF--
Fatal error: Declaration of Test::__construct() must be compatible with TestInterface::__construct(ExampleClass $var) in %s on line %d
