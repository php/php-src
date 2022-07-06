--TEST--
unset() CV 5 (indirect unset() of global variable in session_start())
--INI--
session.auto_start=0
session.save_handler=files
--SKIPIF--
<?php

include(__DIR__.'/../../ext/session/tests/skipif.inc');

?>
--FILE--
<?php
$_SESSION = "ok\n";
session_start();
echo $_SESSION;
echo "\nok\n";
?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
Array
ok
