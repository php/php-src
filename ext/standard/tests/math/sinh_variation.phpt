--TEST--
Test variations in usage of sinh()
--INI--
serialize_precision = 10
--FILE--
<?php
/*
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
        null,
        true,
        false);

for ($i = 0; $i < count($values); $i++) {
    $res = sinh($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(4872401723)
float(-4872401723)
float(7641446995)
float(-7641446995)
float(4872401723)
float(4872401723)
float(4872401723)
float(7641446995)
float(7641446995)
float(INF)
float(0)
float(1.175201194)
float(0)
