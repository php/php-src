--TEST--
Test wrong number of arguments and wrong arg types for ob_start()
--FILE--
<?php
/*
 * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
 * Function is implemented in main/output.c
*/

function justPrint($str) {
	return $str;
}

$arg_1 = "justPrint";
$arg_2 = 0;
$arg_3 = false;
$extra_arg = 1;

echo "\nArg 1 wrong type\n";
var_dump(ob_start(1.5));

?>
--EXPECTF--
Arg 1 wrong type

Warning: ob_start(): no array or string given in %s on line 17

Notice: ob_start(): failed to create buffer in %s on line 17
bool(false)
