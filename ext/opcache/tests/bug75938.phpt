--TEST--
Bug #75938: Modulus value not stored in variable
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function borken($columns) {
    $columns = (int) $columns;
    if ($columns < 1) return 0;
    $count	= count([1,2,3,4,5]);
    var_dump($mod = ($count % $columns));
    var_dump($mod);
}
borken(2);
?>
--EXPECT--
int(1)
int(1)
