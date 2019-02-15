--TEST--
Test log() - basic function test log()
--INI--
precision=14
--FILE--
<?php
$values = array(23,
				-23,
				2.345e1,
				-2.345e1,
				0x17,
				027,
				"23",
				"23.45",
				"2.345e1",
				null,
				true,
				false);

echo "\n LOG tests...no base\n";
for ($i = 0; $i < count($values); $i++) {
	$res = log($values[$i]);
	var_dump($res);
}

echo "\n LOG tests...base\n";
for ($i = 0; $i < count($values); $i++) {
	$res = log($values[$i], 4);
	var_dump($res);
}
?>
--EXPECT--
 LOG tests...no base
float(3.1354942159291)
float(NAN)
float(3.1548704948923)
float(NAN)
float(3.1354942159291)
float(3.1354942159291)
float(3.1354942159291)
float(3.1548704948923)
float(3.1548704948923)
float(-INF)
float(0)
float(-INF)

 LOG tests...base
float(2.2617809780285)
float(NAN)
float(2.275758008814)
float(NAN)
float(2.2617809780285)
float(2.2617809780285)
float(2.2617809780285)
float(2.275758008814)
float(2.275758008814)
float(-INF)
float(0)
float(-INF)
