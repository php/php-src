--TEST--
Testing do-while loop
--POST--
--GET--
--FILE--
<?php 
$i=3;
do {
	echo $i;
	$i--;
} while($i>0);
?>
--EXPECT--
321
