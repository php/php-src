--TEST--
Simple While Loop Test
--POST--
--GET--
--FILE--
<?php $a=1; 
  while($a<10):
	echo $a;
	$a++;
  endwhile?>
--EXPECT--
123456789
