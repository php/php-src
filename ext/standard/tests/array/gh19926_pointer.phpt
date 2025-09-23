--TEST--
GH-19926 (internal pointer behavior after array_splice)
--FILE--
<?php
$a = [1, 2, 3];
unset($a[0]);
next($a);

echo "Before array_splice: ";
var_dump(current($a));

array_splice($a, 0, 0, [999]);

echo "After array_splice: ";
var_dump(current($a));
?>
--EXPECT--
Before array_splice: int(3)
After array_splice: int(999)
