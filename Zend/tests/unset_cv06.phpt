--TEST--
unset() CV 6 (indirect unset() of global variable in session_unset())
--SKIPIF--
<?php include(dirname(__FILE__).'/../../ext/session/tests/skipif.inc'); ?>
--INI--
session.auto_start=0
session.save_handler=files
--FILE--
<?php
session_start();
$_SESSION['x'] = "1\n";
echo $_SESSION['x'];

session_unset();
echo $_SESSION['x'];
echo "ok\n";
?>
--EXPECTF--
1

Notice: Undefined index: x in %sunset_cv06.php on line %d
ok
