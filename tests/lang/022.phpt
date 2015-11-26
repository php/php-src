--TEST--
Switch test 3
--FILE--
<?php 

function switchtest ($i, $j)
{
	switch ($i) {
		case 0:
				switch($j) {
					case 0:
						echo "zero";
						break;
					case 1: 
						echo "one";
						break;
					default:
						echo $j;
						break;
				}
				echo "\n";
				break;
		default:
				echo "Default taken\n";
	}
}
for ($i=0; $i<3; $i++) {
  for ($k=0; $k<10; $k++) {
    switchtest (0,$k);
  }
}
?>
--EXPECT--
zero
one
2
3
4
5
6
7
8
9
zero
one
2
3
4
5
6
7
8
9
zero
one
2
3
4
5
6
7
8
9
