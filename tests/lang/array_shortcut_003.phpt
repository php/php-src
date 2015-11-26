--TEST--
Testing array shortcut and bracket operator
--FILE--
<?php
$a = [1, 2, 3, 4, 5];
print_r([$a[1], $a[3]]);
?>
--EXPECT--
Array
(
    [0] => 2
    [1] => 4
)
