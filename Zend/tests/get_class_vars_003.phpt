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

	protected function __construct() {
		var_dump(get_class_vars('C'));
	}
}

class C extends B {
	public $aaa = 7;
	private $bbb = 8;
	protected $ccc = 9;

	public function __construct() {
		parent::__construct();
	}
}

new C;

?>
--EXPECT--
array(6) {
  ["aaa"]=>
  int(7)
  ["ccc"]=>
  int(9)
  ["a"]=>
  int(1)
  ["aa"]=>
  int(4)
  ["bb"]=>
  int(5)
  ["cc"]=>
  int(6)
}
