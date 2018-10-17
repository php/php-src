--TEST--
Bug #34879 (str_replace, array_map corrupt negative array indexes on 64-bit platforms)
--FILE--
<?php
print_r(str_replace('a', 'b', array(-1 =>-1)));
?>
--EXPECT--
Array
(
    [-1] => -1
)
