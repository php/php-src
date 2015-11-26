--TEST--
get_class_vars(): Testing the scope
--FILE--
<?php

class A {
	public $a = 1;
	private $b = 2;
	private $c = 3;
}

class B extends A {
	static public $aa = 4;
	static private $bb = 5;
	static protected $cc = 6;
}

class C extends B {
	public function __construct() {
		var_dump(get_class_vars('A'));
		var_dump(get_class_vars('B'));
		
		var_dump($this->a, $this->b, $this->c);
	}	
}

new C;

?>
--EXPECTF--
array(1) {
  ["a"]=>
  int(1)
}
array(3) {
  ["a"]=>
  int(1)
  ["aa"]=>
  int(4)
  ["cc"]=>
  int(6)
}

Notice: Undefined property: C::$b in %s on line %d

Notice: Undefined property: C::$c in %s on line %d
int(1)
NULL
NULL
