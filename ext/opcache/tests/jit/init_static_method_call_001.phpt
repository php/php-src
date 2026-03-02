--TEST--
JIT INIT_STATIC_METHOD_CALL: 001 Invalid scope
--INI--
opcache.enable=1
opcache.enable_cli=1
--EXTENSIONS--
opcache
--FILE--
<?php
class C {
	public static function foo() {
		$f = function() {
			return call_user_func(self::bar(), 1, 2, 3, 4, 5);
		};
		return $f();
	}
	public static function bar() {
		return function($a, $b, $c, $d, $e) {return $a + $b + $c + $d + $e;};
	}
}
var_dump(C::foo());
?>
--EXPECT--
int(15)
