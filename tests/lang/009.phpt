--TEST--
Testing function parameter passing
--POST--
--GET--
--FILE--
<?php  old_function Test $a,$b (
		echo $a+$b;	
	);
	Test(1,2)?>
--EXPECT--
3
