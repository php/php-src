--TEST--
Test variations in usage of cos()
--INI--
serialize_precision = 10
--FILE--
<?php
/*
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
        true,
        false);

for ($i = 0; $i < count($values); $i++) {
    $res = cos($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(-0.5328330203)
float(-0.5328330203)
float(-0.1117112391)
float(-0.1117112391)
float(-0.5328330203)
float(-0.5328330203)
float(-0.5328330203)
float(-0.1117112391)
float(-0.1117112391)
float(0.5623790763)
float(0.5403023059)
float(1)
