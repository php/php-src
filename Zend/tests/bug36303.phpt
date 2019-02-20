--TEST--
Bug #36303 (foreach on error_zval produces segfault)
--FILE--
<?php
$x="test";
foreach($x->a->b as &$v) {
}
echo "ok\n";
?>
--EXPECTF--
Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to modify property 'b' of non-object in %s on line %d

Warning: Invalid argument supplied for foreach() in %s on line %d
ok
