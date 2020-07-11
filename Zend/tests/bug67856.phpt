--TEST--
Bug #67856 (Leak when using array_reduce with by-ref function)
--FILE--
<?php
$array = [1, 2, 3];
var_dump(array_reduce($array, function(&$a, &$b) {
    return $a + $b;
}, 0));
?>
--EXPECTF--
Warning: {closure}(): Argument #1 ($a) must be passed by reference, value given in %s on line %d

Warning: {closure}(): Argument #2 ($b) must be passed by reference, value given in %s on line %d

Warning: {closure}(): Argument #1 ($a) must be passed by reference, value given in %s on line %d

Warning: {closure}(): Argument #2 ($b) must be passed by reference, value given in %s on line %d

Warning: {closure}(): Argument #1 ($a) must be passed by reference, value given in %s on line %d

Warning: {closure}(): Argument #2 ($b) must be passed by reference, value given in %s on line %d
int(6)
