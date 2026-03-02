--TEST--
unset() CV 2 (unset() global variable in $GLOBALS)
--FILE--
<?php
$x = "ok\n";
echo $x;
unset($GLOBALS["x"]);
echo $x;
?>
--EXPECTF--
ok

Warning: Undefined variable $x in %s on line %d
