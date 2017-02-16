--TEST--
Bug #50892 (Protected members of non-congruent children classes are visible)
--FILE--
<?php
class foo {
	public $public = "a";	
	private $private = "b";
	protected $protected = "protected";
}

class bar extends foo {
	
}

class kid extends foo {
	public function test() {
		$b = new bar();
		var_dump(get_object_vars($b));
		var_dump($b->protected);
	}
}

$k = new kid();
$k->test();
?>
--EXPECTF--
array(1) {
  ["public"]=>
  string(1) "a"
}

Fatal error: Uncaught Error: Cannot access protected property bar::$protected in %s
Stack trace:
#0 %s: kid->test()
#1 {main}
  thrown in %s
