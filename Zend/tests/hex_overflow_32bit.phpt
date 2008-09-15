--TEST--
testing integer overflow (32bit)
--INI--
precision=14
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

$doubles = array(
	0x1736123FFFAAA,
	0XFFFFFFFFFFFFFFFFFF,
	0xAAAAAAAAAAAAAAEEEEEEEEEBBB,
	0x66666666666666666777777,
	);

foreach ($doubles as $d) {
	$l = $d;
	var_dump($l);
}

echo "Done\n";
?>
--EXPECTF--
float(4.0833602971%dE+14)
float(4.7223664828%dE+21)
float(1.3521606402%dE+31)
float(1.9807040628%dE+27)
Done
