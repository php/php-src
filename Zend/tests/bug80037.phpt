--TEST--
Bug #80037: Typed property must not be accessed before initialization when __get() declared
--FILE--
<?php

final class A
{
	public string $a;

	public static function fromArray(array $props): self
	{
		$me = new static;
		foreach ($props as $k => &$v) {
			$me->{$k} = &$v;  # try to remove &
		}
		return $me;
	}

	public function __get($name)
	{
		throw new \LogicException("Property '$name' is not defined.");
	}
}

var_dump(A::fromArray(['a' => 'foo']));

?>
--EXPECT--
object(A)#1 (1) {
  ["a"]=>
  string(3) "foo"
}
