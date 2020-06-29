--TEST--
Test variations in usage of deg2rad()
--INI--
precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test deg2rad with a different input values

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
        null,
        true,
        false);

for ($i = 0; $i < count($values); $i++) {
    $res = deg2rad($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(0.40142572795869574)
float(-0.40142572795869574)
float(0.40927970959267024)
float(-0.40927970959267024)
float(0.40142572795869574)
float(0.40142572795869574)
float(0.40142572795869574)
float(0.40927970959267024)
float(0.40927970959267024)
float(17.453292519943293)
float(0)
float(0.017453292519943295)
float(0)
