--TEST--
Testing function parameter passing with a return value
--POST--
--GET--
--FILE--
<?php  old_function Test $b (
		$b++;
		return($b);
	);
	$a = Test(1);
	echo $a?>
--EXPECT--
2
