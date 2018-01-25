--TEST--
Execution ordering with comparison operators.
--FILE--
<?php
function f($x) {
	echo "f($x) ";
	return $x;
}

echo "Function call args:\n";
var_dump(f($i=0) < f(++$i));
var_dump(f($i=0) <= f(++$i));
var_dump(f($i=0) > f(++$i));
var_dump(f($i=0) >= f(++$i));

echo "\nArray indices:\n";
$a[1][2] = 0;
$a[3][4] = 1;
$i=0;
var_dump($a[$i=1][++$i] < $a[++$i][++$i]);
var_dump($a[$i=1][++$i] <= $a[++$i][++$i]);
var_dump($a[$i=1][++$i] > $a[++$i][++$i]);
var_dump($a[$i=1][++$i] >= $a[++$i][++$i]);
?>
--EXPECTF--
Function call args:
f(0) f(1) bool(true)
f(0) f(1) bool(true)
f(0) f(1) bool(false)
f(0) f(1) bool(false)

Array indices:
bool(true)
bool(true)
bool(false)
bool(false)
