--TEST--
func_num_args() tests
--FILE--
<?php

function test1() {
	var_dump(func_num_args());
}

function test2($a) {
	var_dump(func_num_args());
}

function test3($a, $b) {
	var_dump(func_num_args());
}

test1();
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
		var_dump(func_num_args());
	}
}

test::test1(1);
var_dump(func_num_args());

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(1)
Exception: Too few arguments to function test2(), 0 passed in %s001.php on line 18 and exactly 1 expected
int(2)
int(0)
Exception: Too few arguments to function test3(), 1 passed in %s001.php on line 27 and exactly 2 expected
int(2)
int(1)

Warning: func_num_args():  Called from the global scope - no function context in %s on line %d
int(-1)
Done
