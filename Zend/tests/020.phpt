--TEST--
func_get_arg() invalid usage
--FILE--
<?php

var_dump(func_get_arg(1,2,3));
var_dump(func_get_arg(1));
var_dump(func_get_arg());

function bar() {
	var_dump(func_get_arg(1));
}

function foo() {
	bar(func_get_arg(1));
}

foo(1,2);

echo "Done\n";
?>
--EXPECTF--	
bool(false)

Warning: func_get_arg():  Called from the global scope - no function context in %s on line %d
bool(false)
bool(false)

Warning: func_get_arg():  Argument 1 not passed to function in %s on line %d
bool(false)
Done
