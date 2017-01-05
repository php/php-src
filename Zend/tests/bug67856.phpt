--TEST--
Bug #67856 (Leak when using array_reduce with by-ref function)
--FILE--
<?php
$array = [1, 2, 3];
var_dump(array_reduce($array, function(&$a, &$b) {
    return $a + $b;
}, 0));
?>
--EXPECT--
int(6)
