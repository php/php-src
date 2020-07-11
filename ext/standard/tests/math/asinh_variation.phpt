--TEST--
Test variations in usage of asinh()
--INI--
serialize_precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test asinh with a different input values

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
    $res = asinh($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(3.829113652)
float(-3.829113652)
float(3.848471992)
float(-3.848471992)
float(3.829113652)
float(3.829113652)
float(3.829113652)
float(3.848471992)
float(3.848471992)
float(7.60090271)
float(0)
float(0.881373587)
float(0)
