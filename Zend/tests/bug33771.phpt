--TEST--
Bug #33771 (error_reporting falls to 0 when @ was used inside try/catch block)
--FILE--
<?php

error_reporting(E_ALL);

var_dump(error_reporting());

function make_exception()
{
    throw new Exception();
}

function make_exception_and_change_err_reporting()
{
    error_reporting(E_ALL & ~E_NOTICE);
    throw new Exception();
}


try {
    @make_exception();
} catch (Exception $e) {}

var_dump(error_reporting());

try {
    @make_exception_and_change_err_reporting();
} catch (Exception $e) {}

var_dump(error_reporting());

echo "Done\n";
?>
--EXPECT--
int(32767)
int(32767)
int(32759)
Done
