--TEST--
unset() CV 1 (unset() global variable)
--FILE--
<?php
$x = "ok\n";
echo $x;
unset($x);
echo $x;
?>
--EXPECTF--
ok

Notice: Undefined variable: x in %sunset_cv01.php on line %d
