--TEST--
Testing call_user_func with self call in same class (not parent)
--FILE--
<?php
 
class A {
	public static function func($str) {
		var_dump(__METHOD__ .': '. $str);
	}
	public function run($str) {
		call_user_func(array($this, 'self::func'), $str);
		call_user_func('self::func', $str);
	}
}

class B extends A {
	public static function func($str) {
		print "This method shouldn't be called when using self::func!\n";
	}
}

$c = new B;
$c->run('This should work!');

?>
--EXPECTF--
%unicode|string%(26) "A::func: This should work!"
%unicode|string%(26) "A::func: This should work!"

