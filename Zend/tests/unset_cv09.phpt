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

Warning: Undefined variable $x in %s on line %d
ok
