--TEST--
Nested foreach by reference on the same array
--FILE--
<?php
$a = [1,2,3];
foreach($a as &$x) {
	foreach($a as &$y) {
		echo "$x-$y\n";
		$y++;
	}
}
?>
--EXPECT--
1-1
2-2
2-3
3-2
3-3
4-4
5-3
5-4
5-5
