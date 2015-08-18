--TEST--
Trying to access undeclared static property
--FILE--
<?php

class bar {
	public function __set($a, $b) {
		print "hello\n";
	}
}

class foo extends bar {
	public function __construct() {
		static::$f = 1;
	}
	public function __set($a, $b) {
		print "foo\n";
	}
}


new foo;

?>
--EXPECTF--
Fatal error: Uncaught Error: Access to undeclared static property: foo::$f in %s:%d
Stack trace:
#0 %s(%d): foo->__construct()
#1 {main}
  thrown in %s on line %d
