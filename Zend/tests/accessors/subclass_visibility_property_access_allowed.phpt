--TEST--
ZE2 Visibility of the property is bypassed and visibility of accessor is allowed in subclass
--FILE--
<?php

class Foo {
	private $bar {
		public get {
			echo __METHOD__.'() = ';
			return $this->bar;
		}
		public set {
			echo __METHOD__."($value)".PHP_EOL;
			$this->bar = $value;
		}
	}
	
	public function __construct() {
		echo __CLASS__.'::__construct()'.PHP_EOL;
		$this->bar = 20;
	}
	
	public function getbar() {
		return $this->bar;
	}
}

class SubFoo extends Foo {
}

$foo = new Foo();
echo '$foo->bar = '; 		var_dump($foo->bar);
echo '$foo->getbar() = '; 	var_dump($foo->getbar());

echo PHP_EOL.'-------------------------'.PHP_EOL.PHP_EOL;

$subfoo = new SubFoo();
echo '$subfoo->getbar() = '; 	var_dump($subfoo->getbar());
echo '$subfoo->bar = ';			var_dump($subfoo->bar);
echo '$subfoo->bar = 10;'.PHP_EOL;
$subfoo->bar = 10;
echo '$subfoo->bar = ';			var_dump($subfoo->bar);
echo '$subfoo->getbar() = '; 	var_dump($subfoo->getbar());

var_dump($subfoo);

?>
==DONE==
--EXPECTF--
Foo::__construct()
Foo::$bar->set(20)
$foo->bar = Foo::$bar->get() = int(20)
$foo->getbar() = Foo::$bar->get() = int(20)

-------------------------

Foo::__construct()
Foo::$bar->set(20)
$subfoo->getbar() = Foo::$bar->get() = int(20)
$subfoo->bar = Foo::$bar->get() = int(20)
$subfoo->bar = 10;
Foo::$bar->set(10)
$subfoo->bar = Foo::$bar->get() = int(10)
$subfoo->getbar() = Foo::$bar->get() = int(10)
object(SubFoo)#2 (1) {
  ["bar":"Foo":private]=>
  int(10)
}
==DONE==
