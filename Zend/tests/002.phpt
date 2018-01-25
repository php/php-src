--TEST--
func_get_arg() tests
--FILE--
<?php

function test1() {
	var_dump(func_get_arg(-10));
	var_dump(func_get_arg(0));
	var_dump(func_get_arg(1));
}

function test2($a) {
	var_dump(func_get_arg(0));
	var_dump(func_get_arg(1));
}

function test3($a, $b) {
	var_dump(func_get_arg(0));
	var_dump(func_get_arg(1));
	var_dump(func_get_arg(2));
}

test1();
test1(10);
test2(1);
try {
	test2();
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
test3(1,2);

call_user_func("test1");
try {
	call_user_func("test3", 1);
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
call_user_func("test3", 1, 2);

class test {
	static function test1($a) {
		var_dump(func_get_arg(0));
		var_dump(func_get_arg(1));
	}
}

test::test1(1);
var_dump(func_get_arg(1));

echo "Done\n";
?>
--EXPECTF--
Warning: func_get_arg():  The argument number should be >= 0 in %s on line %d
bool(false)

Warning: func_get_arg():  Argument 0 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  The argument number should be >= 0 in %s on line %d
bool(false)
int(10)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)
int(1)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)
Exception: Too few arguments to function test2(), 0 passed in %s002.php on line %d and exactly 1 expected
int(1)
int(2)

Warning: func_get_arg():  Argument 2 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  The argument number should be >= 0 in %s on line %d
bool(false)

Warning: func_get_arg():  Argument 0 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)
Exception: Too few arguments to function test3(), 1 passed in %s002.php on line %d and exactly 2 expected
int(1)
int(2)

Warning: func_get_arg():  Argument 2 not passed to function in %s on line %d
bool(false)
int(1)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  Called from the global scope - no function context in %s on line %d
bool(false)
Done
