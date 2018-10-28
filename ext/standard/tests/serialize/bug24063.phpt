--TEST--
Bug #24063 (serialize() missing 0 after the . on scientific notation)
--INI--
serialize_precision=100
precision=12
--FILE--
<?php
$v = 1;
for ($i = 1; $i < 10; $i++) {
	$v /= 10;
	echo "{$v} ".unserialize(serialize($v))."\n";
}
?>
--EXPECT--
0.1 0.1
0.01 0.01
0.001 0.001
0.0001 0.0001
1.0E-5 1.0E-5
1.0E-6 1.0E-6
1.0E-7 1.0E-7
1.0E-8 1.0E-8
1.0E-9 1.0E-9
