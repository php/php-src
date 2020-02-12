--TEST--
Test variations in usage of cos()
--INI--
precision = 10
--FILE--
<?php
/*
 * proto float cos(float number)
 * Function is implemented in ext/standard/math.c
*/


//Test cos with a different input values

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
    $res = cos($values[$i]);
    var_dump($res);
}

?>
--EXPECTF--
float(-0.5328330203333975)
float(-0.5328330203333975)
float(-0.11171123911915933)
float(-0.11171123911915933)
float(-0.5328330203333975)
float(-0.5328330203333975)
float(-0.5328330203333975)
float(-0.11171123911915933)
float(-0.11171123911915933)
float(0.5623790762907029)

Notice: A non well formed numeric value encountered in %s on line %d
float(0.5623790762907029)
float(1)
float(0.5403023058681398)
float(1)
