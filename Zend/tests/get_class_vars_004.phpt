--TEST--
get_class_vars(): Testing the scope
--FILE--
<?php

class A {
	public $a = 1;
	static public $A = 2;

	private $b = 3;
	static private $B = 4;

	protected $c = 5;
	static protected $C = 6;
	
	public function __construct() {
		var_dump(get_class_vars('A'));
	}
	
	static public function test() {
		var_dump(get_class_vars('A'));
	}
}

var_dump(get_class_vars('A'));

new A;

var_dump(A::test());

?>
--EXPECT--
array(2) {
  [u"a"]=>
  int(1)
  [u"A"]=>
  int(2)
}
array(6) {
  [u"a"]=>
  int(1)
  [u"b"]=>
  int(3)
  [u"c"]=>
  int(5)
  [u"A"]=>
  int(2)
  [u"B"]=>
  int(4)
  [u"C"]=>
  int(6)
}
array(6) {
  [u"a"]=>
  int(1)
  [u"b"]=>
  int(3)
  [u"c"]=>
  int(5)
  [u"A"]=>
  int(2)
  [u"B"]=>
  int(4)
  [u"C"]=>
  int(6)
}
NULL
