--TEST--
Bug #33771 (error_reporting falls to 0 when @ was used inside try/catch block)
--FILE--
<?php

error_reporting(E_ALL | E_STRICT);

var_dump(error_reporting());

function make_exception()
{
    throw new Exception();
}

function make_exception_and_change_err_reporting()
{
    error_reporting(E_ALL & ~E_STRICT);
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
--EXPECTF--	
int(8191)
int(8191)
int(6143)
Done
