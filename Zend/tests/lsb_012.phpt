--TEST--
ZE2 Late Static Binding call to static::method() from internal function (string)
--FILE--
<?php

class Test1 {
	static function ok() {
		echo "bug";
	}
	static function test() {
		call_user_func("static::ok");
	}
}

class Test2 extends Test1 {
	static function ok() {
		echo "ok";
	}
}
Test2::test();
?>
--EXPECT--
ok
