--TEST--
testing integer overflow (32bit)
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

$doubles = array(
	2147483648,
	2147483649,
	2147483658,
	2147483748,
	2147484648,
	);

foreach ($doubles as $d) {
	$l = (int)$d;
	var_dump($l);
}

echo "Done\n";
?>
--EXPECTF--	
int(-2147483648)
int(-2147483647)
int(-2147483638)
int(-2147483548)
int(-2147482648)
Done
