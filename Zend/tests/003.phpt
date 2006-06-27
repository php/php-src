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
test2();
test3(1,2);

call_user_func("test1");
call_user_func("test3", 1);
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

Warning: Missing argument 1 for test2(), called in %s on line %d and defined in %s on line %d
array(0) {
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
array(0) {
}

Warning: Missing argument 2 for test3() in %s on line %d
array(1) {
  [0]=>
  int(1)
}
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
