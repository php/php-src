--TEST--
array_pop() function precerve foreach by reference iterator pointer
--FILE--
<?php
$a = [1,2,3];
foreach($a as &$v) {
	echo "$v\n";
	if ($v == 2) {
		array_pop($a);
	}
}
?>
--EXPECT--
1
2
