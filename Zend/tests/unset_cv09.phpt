--TEST--
unset() CV 9 (unset() of global variable in array_pop($GLOBALS))
--FILE--
<?php
$x = "ok\n";
echo array_pop($GLOBALS);
echo $x;
echo "ok\n";
?>
--EXPECTF--
ok

Notice: Undefined variable: x in %sunset_cv09.php on line %d
ok
