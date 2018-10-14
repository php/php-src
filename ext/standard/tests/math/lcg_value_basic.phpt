--TEST--
Maths test for xapic versions of lcg_value()
--FILE--
<?php

echo "MATHS test script started\n";


echo "\n lcg_value tests...\n";
for ($i = 0; $i < 100; $i++) {
	$res = lcg_value();

	if (!is_float($res) || $res < 0 || $res > 1) {
		break;
	}
}

if ($i != 100) {
	echo "FAILED\n";
} else {
	echo "PASSED\n";
}

echo "\n lcg_value error cases..spurious args get ignored\n";
$res = lcg_value(23);

if (!is_float($res) || $res < 0 || $res > 1) {
	echo "FAILED\n";
} else {
	echo "PASSED\n";
}

$res = lcg_value(10,false);
if (!is_float($res) || $res < 0 || $res > 1) {
	echo "FAILED\n";
} else {
	echo "PASSED\n";
}

echo "MATHS test script completed\n";

?>
--EXPECT--
MATHS test script started

 lcg_value tests...
PASSED

 lcg_value error cases..spurious args get ignored
PASSED
PASSED
MATHS test script completed
