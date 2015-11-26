--TEST--
Bug #27457 (Problem with strtr() and translation array)
--FILE--
<?php
	$test = "Dot in brackets [.]\n";
	echo $test;
	$test = strtr($test, array('.' => '0'));
	echo $test;
	$test = strtr($test, array('0' => '.'));
	echo $test;
	$test = strtr($test, '.', '0');
	echo $test;
	$test = strtr($test, '0', '.');
	echo $test;
?>
--EXPECT--
Dot in brackets [.]
Dot in brackets [0]
Dot in brackets [.]
Dot in brackets [0]
Dot in brackets [.]
