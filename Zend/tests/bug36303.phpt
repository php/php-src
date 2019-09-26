--TEST--
Bug #36303 (foreach on error_zval produces segfault)
--FILE--
<?php
$x = [];
foreach ($x[[]] as &$v) {
}
echo "ok\n";
?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
ok
