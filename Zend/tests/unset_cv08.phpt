--TEST--
unset() CV 8 (unset() of global variable in array_unique($GLOBALS))
--FILE--
<?php
$a = "ok\n";
$b = "ok\n";
array_unique($GLOBALS);
echo $a;
echo $b;
echo "ok\n";
?>
--EXPECTF--
ok
ok
ok
