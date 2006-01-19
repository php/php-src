--TEST--
Bug #36071 (Engine Crash related with 'clone')
--INI--
error_reporting=4095
--FILE--
<?php
$a = clone 0;
$a[0]->b = 0;
echo "ok\n";
?>
--EXPECTF--
Warning: __clone method called on non-object in %sbug36071.php on line 2
ok