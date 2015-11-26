--TEST--
sort() functions precerve foreach by reference iterator pointer
--FILE--
<?php
$a = [1,2,3,4,5,0];
foreach($a as &$v) {
	echo "$v\n";
	if ($v == 3) {
		rsort($a);
	}
}
?>
--EXPECT--
1
2
3
2
1
0
