--TEST--
Testing $argc and $argv handling
--POST--
--GET--
ab+cd+ef+123+test
--FILE--
<?php 
	for($i=0;$i<$argc;$i++) {
		echo "$i: ".$argv[$i]."\n";
	}
?>
--EXPECT--
0: ab
1: cd
2: ef
3: 123
4: test
