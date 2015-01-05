--TEST--
Check that passing a parameter to toArray() produces a correct error
--CREDITS--
PHPNW Testfest 2009 - Paul Court ( g@rgoyle.com )
--FILE--
<?php
	// Create a fixed array
	$fixedArray = new SplFixedArray(5);
	
	// Fill it up
	for ($i=0; $i < 5; $i++) { 
		$fixedArray[$i] = "PHPNW Testfest";
	}
	
	// Test count() returns correct error when parameters are passed.
	$fixedArray->count(1);
?>
--EXPECTF--
Warning: SplFixedArray::count() expects exactly 0 parameters, %d given in %s on line %d
