--TEST--
Bug #70321 (Magic getter breaks reference to array property)
--FILE--
<?php

class foo {
	private $bar;
	public function __construct() {
		$this->bar = new bar();
	}

	public function &__get($key) {
		$bar = $this->bar;
		// no direct reference to $this->bar
		return $bar;
	}
}

class bar { public $baz = []; }

$foo = new foo();
$foo->bar->baz[] = function() {};
var_dump($foo->bar->baz);

?>
--EXPECTF--
array(1) {
  [0]=>
  object(Closure)#%d (0) {
  }
}
