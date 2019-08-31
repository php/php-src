--TEST--
Test typed properties yield reference guard
--FILE--
<?php
$foo = new class {
	public int $foo = 1;
	public int $bar = 3; 
	public int $baz = 5;
	public int $qux = PHP_INT_MAX;

	public function &fetch() {
		yield $this->foo;
		yield $this->bar;
		yield $this->baz;
		yield $this->qux;
	}
};

try {
	foreach ($foo->fetch() as &$prop) {
		$prop += 1;
	}
} catch (Error $e) { echo $e->getMessage(), "\n"; }

var_dump($foo);
?>
--EXPECTF--
Cannot assign float to reference held by property class@anonymous::$qux of type int
object(class@anonymous)#1 (4) {
  ["foo"]=>
  int(2)
  ["bar"]=>
  int(4)
  ["baz"]=>
  int(6)
  ["qux"]=>
  &int(%d)
}
