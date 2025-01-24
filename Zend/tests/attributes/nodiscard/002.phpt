--TEST--
#[\NoDiscard]: __call(), __callStatic(), and __invoke().
--FILE--
<?php

class Clazz {
	#[\NoDiscard]
	public function __call(string $name, array $args): int {
		echo "__call({$name})", PHP_EOL;

		return strlen($name);
	}

	#[\NoDiscard]
	public static function __callStatic(string $name, array $args): int {
		echo "__callStatic({$name})", PHP_EOL;

		return strlen($name);
	}

	#[\NoDiscard]
	public function __invoke(string $param): int {
		echo "__invoke({$param})", PHP_EOL;

		return strlen($param);
	}
}

$cls = new Clazz();
$cls->test();
Clazz::test();
$cls('foo');

?>
--EXPECTF--
Warning: (A)The return value of method Clazz::test() is expected to be consumed in %s on line %d
__call(test)

Warning: (A)The return value of method Clazz::test() is expected to be consumed in %s on line %d
__callStatic(test)

Warning: (A)The return value of method Clazz::__invoke() is expected to be consumed in %s on line %d
__invoke(foo)

