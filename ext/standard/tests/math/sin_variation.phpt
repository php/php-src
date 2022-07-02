--TEST--
Test variations in usage of sin()
--INI--
serialize_precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test sin with a different input values

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
    $res = sin($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(-0.8462204042)
float(0.8462204042)
float(-0.9937407102)
float(0.9937407102)
float(-0.8462204042)
float(-0.8462204042)
float(-0.8462204042)
float(-0.9937407102)
float(-0.9937407102)
float(0.8268795405)
float(0.8414709848)
float(0)
