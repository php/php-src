--TEST--
Bug #24054 (Assignment operator *= broken)
--FILE--
<?php

	$i = 10000000;
	$i *= 1001;

	$j = 10000000;
	$j = $j * 1001;

	var_dump($i,$j);

?>
--EXPECT--
float(1.001E+10)
float(1.001E+10)
