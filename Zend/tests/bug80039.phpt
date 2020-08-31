--TEST--
Bug #80039: Illegal string offset and Cannot create references to/from string offsets
--FILE--
<?php

final class A
{
	public string $a;

	public static function fromArray(array $props): self
	{
		$me = new static;
		foreach ($props as $k => &$v) {
			$me->{$k} = &$v;
		}
		return $me;
	}

	public function __get($name)
	{
		throw new \LogicException("Property '$name' is not defined.");
	}
}

class ObjectHelpers
{
	public static function hasProperty(string $class, string $name)
	{
		static $cache = [];
		$prop = &$cache[$class][$name];  # <-- emits error
        var_dump($prop);
	}
}

set_exception_handler(function ($e) {
	ObjectHelpers::hasProperty(A::class, 'a');
});

A::fromArray(['a' => 'foo']);

?>
--EXPECT--
NULL
