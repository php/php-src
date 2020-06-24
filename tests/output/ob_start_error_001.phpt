--TEST--
Test wrong number of arguments and wrong arg types for ob_start()
--FILE--
<?php
/*
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

Warning: ob_start(): no array or string given in %s on line %d

Notice: ob_start(): Failed to create buffer in %s on line %d
bool(false)
