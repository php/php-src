--TEST--
GH-17913: ReflectionClassConstant::isDeprecated() with __call() and __callStatic()
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

$foo = new Clazz;
$closure = Closure::fromCallable([$foo, 'test']);

$rc = new ReflectionFunction($closure);
var_dump($rc->getAttributes()[0]->newInstance());
var_dump($rc->isDeprecated());

$closure = $foo->test(...);

$rc = new ReflectionFunction($closure);
var_dump($rc->getAttributes()[0]->newInstance());
var_dump($rc->isDeprecated());

$closure = Closure::fromCallable('Clazz::test');

$rc = new ReflectionFunction($closure);
var_dump($rc->getAttributes()[0]->newInstance());
var_dump($rc->isDeprecated());

$closure = Clazz::test(...);

$rc = new ReflectionFunction($closure);
var_dump($rc->getAttributes()[0]->newInstance());
var_dump($rc->isDeprecated());

?>
--EXPECTF--
object(Deprecated)#%d (2) {
  ["message"]=>
  NULL
  ["since"]=>
  NULL
}
bool(true)
object(Deprecated)#%d (2) {
  ["message"]=>
  NULL
  ["since"]=>
  NULL
}
bool(true)
object(Deprecated)#%d (2) {
  ["message"]=>
  string(18) "due to some reason"
  ["since"]=>
  NULL
}
bool(true)
object(Deprecated)#%d (2) {
  ["message"]=>
  string(18) "due to some reason"
  ["since"]=>
  NULL
}
bool(true)
