--TEST--
Clone with supports __clone
--FILE--
<?php

class Clazz {
	public function __construct(
		public string $foo,
		public string $bar,
	) { }

	public function __clone() {
		$this->foo = 'foo updated in __clone';
		$this->bar = 'bar updated in __clone';
	}
}

$c = new Clazz('foo', 'bar');

var_dump(clone($c, [ 'foo' => 'foo updated in clone-with' ]));

?>
--EXPECTF--
object(Clazz)#%d (2) {
  ["foo"]=>
  string(25) "foo updated in clone-with"
  ["bar"]=>
  string(22) "bar updated in __clone"
}
