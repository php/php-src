--TEST--
Bug #19943 (memleaks)
--FILE--
<?php
	$ar = array();
	for ($count = 0; $count < 10; $count++) {
		$ar[$count]        = "$count";
		$ar[$count]['idx'] = "$count";
	}

	for ($count = 0; $count < 10; $count++) {
		echo $ar[$count]." -- ".$ar[$count]['idx']."\n";
	}
	$a = "0123456789";
	$a[9] = $a{0};
	var_dump($a);
?>
--EXPECT--
0 -- 0
1 -- 1
2 -- 2
3 -- 3
4 -- 4
5 -- 5
6 -- 6
7 -- 7
8 -- 8
9 -- 9
string(10) "0123456780"
