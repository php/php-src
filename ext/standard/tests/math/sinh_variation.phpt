--TEST--
Test variations in usage of sinh()
--INI--
precision = 10
--FILE--
<?php
/*
 * proto float sinh(float number)
 * Function is implemented in ext/standard/math.c
*/


//Test sinh with a different input values

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
        "1000ABC",
        null,
        true,
        false);

for ($i = 0; $i < count($values); $i++) {
    $res = sinh($values[$i]);
    var_dump($res);
}

?>
--EXPECTF--
float(4872401723.124452)
float(-4872401723.124452)
float(7641446994.979367)
float(-7641446994.979367)
float(4872401723.124452)
float(4872401723.124452)
float(4872401723.124452)
float(7641446994.979367)
float(7641446994.979367)
float(INF)

Notice: A non well formed numeric value encountered in %s on line %d
float(INF)
float(0)
float(1.1752011936438014)
float(0)
