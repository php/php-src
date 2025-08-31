--TEST--
Return by reference from array_reduce() callback
--FILE--
<?php
$array = [1, 2];
var_dump(array_reduce($array, function &($a, $b) {
    return $b;
}, 0));
?>
--EXPECT--
int(2)
