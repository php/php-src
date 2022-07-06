--TEST--
Bug #29896 (Backtrace argument list out of sync)
--FILE--
<?php
function userErrorHandler($num, $msg, $file, $line)
{
    debug_print_backtrace();
}

$OldErrorHandler = set_error_handler("userErrorHandler");

function GenerateError1($A1)
{
    $a = $b;
}

function GenerateError2($A1)
{
    GenerateError1("Test1");
}

GenerateError2("Test2");
?>
--EXPECTF--
#0  userErrorHandler(2, Undefined variable $b, %s, %d) called at [%s:%d]
#1  GenerateError1(Test1) called at [%sbug29896.php:16]
#2  GenerateError2(Test2) called at [%sbug29896.php:19]
