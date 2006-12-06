--TEST--
unset() CV 5 (indirect unset() of global variable in session_start())
--INI--
session.auto_start=0
session.save_handler=files
--SKIPIF--
<?php if (!extension_loaded("session")) die("skip"); ?>
--FILE--
<?php
$_SESSION = "ok\n";
echo $_SESSION;
session_start();
echo $_SESSION;
echo "\nok\n";
?>
--EXPECTF--
ok

Warning: session_start(): Cannot send session cookie - headers already sent by (output started at %sunset_cv05.php on line %d

Warning: session_start(): Cannot send session cache limiter - headers already sent (output started at %sunset_cv05.php:%d) in %sunset_cv05.php on line %d
Array
ok
