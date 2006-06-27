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
test2();
test3(1,2);

call_user_func("test1");
call_user_func("test3", 1);
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

Warning: Missing argument 1 for test2(), called in %s on line %d and defined in %s on line %d

Warning: func_get_arg():  Argument 0 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)
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

Warning: Missing argument 2 for test3() in %s on line %d
int(1)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)

Warning: func_get_arg():  Argument 2 not passed to function in %s on line %d
bool(false)
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
