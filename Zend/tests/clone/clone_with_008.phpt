--TEST--
Clone with readonly
--FILE--
<?php

readonly class Clazz {
	public function __construct(
		public public(set) string $a,
		public public(set) string $b,
	) { }

	public function __clone() {
		$this->b = '__clone';
	}
}

$c = new Clazz('default', 'default');

var_dump(clone($c, [ 'a' => "with" ]));

try {
	var_dump(clone($c, [ 'b' => "with" ]));
} catch (Throwable $e) {
	echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
object(Clazz)#%d (2) {
  ["a"]=>
  string(4) "with"
  ["b"]=>
  string(7) "__clone"
}
object(Clazz)#%d (2) {
  ["a"]=>
  string(7) "default"
  ["b"]=>
  string(4) "with"
}
