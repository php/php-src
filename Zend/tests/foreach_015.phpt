--TEST--
array_shift() function precerve foreach by reference iterator pointer
--FILE--
<?php
$a = [1,2,3,4];
foreach($a as &$v) {
	echo "$v\n";
	array_shift($a);
}
var_dump($a);
?>
--EXPECT--
1
2
3
4
array(0) {
}
