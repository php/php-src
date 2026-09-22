--TEST--
unset() CV 6 (indirect unset() of global variable in session_unset())
--EXTENSIONS--
session
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

Warning: Undefined array key "x" in %s on line %d
ok
