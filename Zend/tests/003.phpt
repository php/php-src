--TEST--
func_get_args() tests
--FILE--
<?php

function test1() {
	var_dump(func_get_args());
}

function test2($a) {
	var_dump(func_get_args());
}

function test3($a, $b) {
	var_dump(func_get_args());
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
		var_dump(func_get_args());
	}
}

test::test1(1);
var_dump(func_get_args());

echo "Done\n";
?>
--EXPECTF--
array(0) {
}
array(1) {
  [0]=>
  int(10)
}
array(1) {
  [0]=>
  int(1)
}
Exception: Too few arguments to function test2(), 0 passed in %s003.php on line %d and exactly 1 expected
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(0) {
}
Exception: Too few arguments to function test3(), 1 passed in %s003.php on line %d and exactly 2 expected
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(1) {
  [0]=>
  int(1)
}

Warning: func_get_args():  Called from the global scope - no function context in %s on line %d
bool(false)
Done
