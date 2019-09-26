--TEST--
Bug #36303 (foreach on error_zval produces segfault)
--FILE--
<?php
$x=[PHP_INT_MAX=>"test"];
foreach ($x[] as &$v) {
}
echo "ok\n";
?>
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
ok
