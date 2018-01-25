--TEST--
testing integer overflow (32bit)
--INI--
precision=14
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

$doubles = array(
	076545676543223,
	032325463734,
	0777777,
	07777777777777,
	033333333333333,
	);

foreach ($doubles as $d) {
	$l = (double)$d;
	var_dump($l);
}

echo "Done\n";
?>
--EXPECTF--
float(4308640384%d)
float(3545655%d)
float(262143)
float(549755813%d)
float(1884877076%d)
Done
