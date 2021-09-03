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
#0 %s(%d): userErrorHandler(2, 'Undefined varia...', '%s', %d)
#1 %s(%d): GenerateError1('Test1')
#2 %s(%d): GenerateError2('Test2')
