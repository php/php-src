--TEST--
Testing do-while loop
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
