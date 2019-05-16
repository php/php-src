--TEST--
Testing function parameter passing
--FILE--
<?php
function test ($a,$b) {
	echo $a+$b;
}
test(1,2);
?>
--EXPECT--
3
