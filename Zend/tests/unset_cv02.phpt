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

Notice: Undefined variable: x in %sunset_cv02.php on line %d
