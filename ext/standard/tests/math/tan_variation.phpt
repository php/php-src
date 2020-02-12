--TEST--
Test variations in usage of tan()
--INI--
precision = 10
--FILE--
<?php
/*
 * proto float tan(float number)
 * Function is implemented in ext/standard/math.c
*/


//Test tan with a different input values

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
    $res = tan($values[$i]);
    var_dump($res);
}

?>
--EXPECTF--
float(1.5881530833912738)
float(-1.5881530833912738)
float(8.895619796255806)
float(-8.895619796255806)
float(1.5881530833912738)
float(1.5881530833912738)
float(1.5881530833912738)
float(8.895619796255806)
float(8.895619796255806)
float(1.4703241557027185)

Notice: A non well formed numeric value encountered in %s on line %d
float(1.4703241557027185)
float(0)
float(1.5574077246549023)
float(0)
