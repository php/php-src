--TEST--
Test variations in usage of log10()
--INI--
precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test log10 with a different input values

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
    $res = log10($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(1.3617278360175928)
float(NAN)
float(1.3701428470511021)
float(NAN)
float(1.3617278360175928)
float(1.3617278360175928)
float(1.3617278360175928)
float(1.3701428470511021)
float(1.3701428470511021)
float(3)
float(-INF)
float(0)
float(-INF)
