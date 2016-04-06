--TEST--
Test typed properties binary assign op helper test
--FILE--
<?php
class Foo {
	public int $bar = 0;

	public function __construct() {
		$this->bar += 2;
	}
}

$foo = new Foo();

var_dump($foo->bar);
?>
--EXPECT--
int(2)







