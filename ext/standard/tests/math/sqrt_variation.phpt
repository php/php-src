--TEST--
Test variations in usage of sqrt()
--INI--
precision = 14
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test sqrt with a different input values
echo "*** Testing sqrt() : usage variations ***\n";

$values = array(23,
        -23,
        2.345e1,
        -2.345e1,
        0x17,
        027,
        "23",
        "23.45",
        "2.345e1",
        "1000",
        true,
        false);

for ($i = 0; $i < count($values); $i++) {
    $res = sqrt($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
*** Testing sqrt() : usage variations ***
float(4.795831523312719)
float(NAN)
float(4.8425200051213)
float(NAN)
float(4.795831523312719)
float(4.795831523312719)
float(4.795831523312719)
float(4.8425200051213)
float(4.8425200051213)
float(31.622776601683793)
float(1)
float(0)
