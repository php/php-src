--TEST--
#[\Deprecated]: Functions and Methods.
--FILE--
<?php

#[\Deprecated]
function test() {
}

#[\Deprecated("use test() instead")]
function test2() {
}

class Clazz {
	#[\Deprecated]
	function test() {
	}

	#[\Deprecated("use test() instead")]
	function test2() {
	}
}

$closure = #[\Deprecated] function() {
};

$closure2 = #[\Deprecated] function() {
};

class Constructor {
	#[\Deprecated]
	public function __construct() {
	}

	#[\Deprecated]
	public function __destruct() {
	}
}

test();
test2();
call_user_func("test");

$cls = new Clazz();
$cls->test();
$cls->test2();

call_user_func([$cls, "test"]);

$closure();

$closure2();

new Constructor();

?>
--EXPECTF--
Deprecated: Function test() is deprecated in %s

Deprecated: Function test2() is deprecated, use test() instead in %s on line %d

Deprecated: Function test() is deprecated in %s on line %d

Deprecated: Method Clazz::test() is deprecated in %s

Deprecated: Method Clazz::test2() is deprecated, use test() instead in %s

Deprecated: Method Clazz::test() is deprecated in %s

Deprecated: Function {closure:%s:%d}() is deprecated in %s on line %d

Deprecated: Function {closure:%s:%d}() is deprecated in %s on line %d

Deprecated: Method Constructor::__construct() is deprecated in %s on line %d

Deprecated: Method Constructor::__destruct() is deprecated in %s on line %d
