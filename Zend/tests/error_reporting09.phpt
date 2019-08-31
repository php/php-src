--TEST--
testing @ and error_reporting - 9
--FILE--
<?php

error_reporting(E_ALL & ~E_DEPRECATED);

function bar() {
	echo @$blah;
	echo $undef2;
}

function foo() {
	echo @$undef;
	error_reporting(E_ALL);
	echo $blah;
	return bar();
}

@foo();

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECTF--
Notice: Undefined variable: blah in %s on line %d

Notice: Undefined variable: undef2 in %s on line %d
int(32767)
Done
