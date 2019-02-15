--TEST--
Bug #76539 (Trait attribute is set incorrectly when using self::class with another string)
--FILE--
<?php
trait MyTrait {
	protected $attr = self::class . 'Test';

	public function test() {
		echo $this->attr, PHP_EOL;
	}
}

class A {
	use MyTrait;
}

class B {
	use MyTrait;
}

(new A())->test();
(new B())->test();
?>
--EXPECT--
ATest
BTest
