--TEST--
Bug #45186.2 (__call depends on __callstatic in class scope)
--FILE--
<?php

class bar  {
	public function __call($a, $b) {
		print "__call:\n";
		var_dump($a);
	}
	public function test() {
		self::ABC();
		bar::ABC();
		call_user_func(array('BAR', 'xyz'));
		call_user_func('BAR::www');
		call_user_func(array('self', 'y'));
		call_user_func('self::y');
	}
	static function x() { 
		print "ok\n";
	}
}

$x = new bar;

$x->test();

call_user_func(array('BAR','x'));
call_user_func('BAR::www');
call_user_func('self::y');

?>
--EXPECTF--
__call:
string(3) "ABC"
__call:
string(3) "ABC"
__call:
string(3) "xyz"
__call:
string(3) "www"
__call:
string(1) "y"
__call:
string(1) "y"
ok

Warning: call_user_func() expects parameter 1 to be a valid callback, class 'bar' does not have a method 'www' in %s on line %d

Warning: call_user_func() expects parameter 1 to be a valid callback, cannot access self:: when no class scope is active in %sbug45186_2.php on line 27
