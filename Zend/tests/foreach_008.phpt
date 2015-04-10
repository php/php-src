--TEST--
Nested foreach by reference and array modification
--FILE--
<?php
$a = [0, 1, 2, 3];
foreach ($a as &$x) {
	foreach ($a as &$y) {
		echo "$x - $y\n";
		if ($x == 0 && $y == 1) {
			unset($a[2]);
			unset($a[1]);
		}
	}
}
?>
--EXPECT--
0 - 0
0 - 1
0 - 3
3 - 0
3 - 3
