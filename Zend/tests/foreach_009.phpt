--TEST--
Nested foreach by reference and array modification with resize
--FILE--
<?php
$a = [0, 1, 2, 3, 4, 5, 6, 7];
unset($a[0], $a[1], $a[2], $a[3]);
foreach ($a as &$ref) {
	foreach ($a as &$ref2) {
		echo "$ref-$ref2\n";
		if ($ref == 5 && $ref2 == 6) {
			$a[42] = 8;
		}
	}
}
?>
--EXPECT--
4-4
4-5
4-6
4-7
5-4
5-5
5-6
5-7
5-8
6-4
6-5
6-6
6-7
6-8
7-4
7-5
7-6
7-7
7-8
8-4
8-5
8-6
8-7
8-8
