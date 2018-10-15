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

echo "\n lcg_value error cases..\n";
$res = lcg_value(23);

if (is_null($res)) {
	echo "PASSED\n";
} else {
	echo "FAILED\n";
}

$res = lcg_value(10,false);
if (is_null($res)) {
	echo "PASSED\n";
} else {
	echo "FAILED\n";
}

echo "MATHS test script completed\n";

?>
--EXPECTF--
MATHS test script started

 lcg_value tests...
PASSED

 lcg_value error cases..

Warning: lcg_value() expects exactly 0 parameters, 1 given in %slcg_value_basic.php on line %d
PASSED

Warning: lcg_value() expects exactly 0 parameters, 2 given in %slcg_value_basic.php on line %d
PASSED
MATHS test script completed
