--TEST--
Mean recursion test
--FILE--
<?php
function RekTest ($nr) {
	echo " $nr ";
	$j=$nr+1;
	while ($j < 10) {
	  echo " a ";
	  RekTest($j);
	  $j++;
	  echo " b $j ";
	}
	echo "\n";
}

RekTest(0);
?>
--EXPECT--
 0  a  1  a  2  a  3  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 4  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 3  a  3  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 4  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 2  a  2  a  3  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 4  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 3  a  3  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 4  a  4  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 5  a  5  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 6  a  6  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 7  a  7  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
 b 8  a  8  a  9 
 b 10 
 b 9  a  9 
 b 10 
