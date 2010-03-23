--TEST--
testing @ and error_reporting - 5
--FILE--
<?php

error_reporting(E_ALL);
	
class test {
	function __get($name) {
		return $undef_name;
	}
	function __set($name, $value) {
		return $undef_value;
	}
}

$test = new test;

$test->abc = 123;
echo $test->bcd;

@$test->qwe = 123;
echo @$test->wer;

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--	
Notice: Undefined variable: undef_value in %s on line %d

Notice: Undefined variable: undef_name in %s on line %d
int(30719)
Done
