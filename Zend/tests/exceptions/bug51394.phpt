--TEST--
Bug #51394 (Error line reported incorrectly if error handler throws an exception)
--INI--
error_reporting=-1
--FILE--
<?php
function eh()
{
    throw new Exception("error!");
    return false;
}

set_error_handler("eh");
$a = $empty($b);
?>
--EXPECTF--
Fatal error: Uncaught Exception: error! in %sbug51394.php:%d
Stack trace:
#0 %s(%d): eh(2, 'Undefined varia...', '%s', %d)
#1 {main}
  thrown in %sbug51394.php on line %d
