--TEST--
Testing magic methods __set, __get and __call in cascade
--FILE--
<?php

class test {
	static public $i = 0;
	
	public function __construct() {
		self::$i++;
	}
	
	public function __set($a, $b) {
		return x();
	}
	
	public function __get($a) {
		return x();
	}
	
	public function __call($a, $b) {
		return x();
	}
}

function x() {
	return new test;
}

x()
	->a
		->b()
			->c	= 1;

var_dump(test::$i);

?>
--EXPECT--
int(4)
