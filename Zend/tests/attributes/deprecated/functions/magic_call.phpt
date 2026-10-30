--TEST--
#[\Deprecated]: __call() and __callStatic()
--FILE--
<?php

class Clazz {
	#[\Deprecated]
	function __call(string $name, array $params) {
	}

	#[\Deprecated("due to some reason")]
	static function __callStatic(string $name, array $params) {
	}
}

$cls = new Clazz();
$cls->test();
Clazz::test2();

?>
--EXPECTF--
Deprecated: Method Clazz::test() is deprecated in %s

Deprecated: Method Clazz::test2() is deprecated, due to some reason in %s on line %d
