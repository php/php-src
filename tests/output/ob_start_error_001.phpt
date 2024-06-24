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
try {
    var_dump(ob_start(1.5));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Arg 1 wrong type
TypeError: ob_start(): Argument #1 ($callback) must be a valid callback or null, function "1.5" not found or invalid function name
