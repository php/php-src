--TEST--
Testing __call and __callstatic with callbacks
--FILE--
<?php

class Foo {
	public function __call($a, $b) {
		print "nonstatic\n";
		var_dump($a);
	}
	static public function __callStatic($a, $b) {
		print "static\n";
		var_dump($a);
	}
}

$a = new Foo;
call_user_func(array($a, 'aAa'));
call_user_func(array('Foo', 'aAa'));

?>
--EXPECTF--
nonstatic
string(3) "aAa"
static
string(3) "aAa"
