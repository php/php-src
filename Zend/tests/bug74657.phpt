--TEST--
Bug #74657 (Undefined constants in array properties result in broken properties)
--FILE--
<?php

interface I {
}

class C {
	const FOO = I::FOO;

	public $options = [self::FOO => "bar"];
}

try {
	var_dump((new C)->options);
} catch (Throwable $e) {}

var_dump((new C)->options);
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined class constant 'I::FOO' in %sbug74657.php:%d
Stack trace:
#0 {main}
  thrown in %sbug74657.php on line %d
