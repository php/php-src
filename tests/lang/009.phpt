--TEST--
Testing function parameter passing
--POST--
--GET--
--FILE--
<?php
function test ($a,$b) {
	echo $a+$b;	
}
test(1,2);
?>
--EXPECT--
3
