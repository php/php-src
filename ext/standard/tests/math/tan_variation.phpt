--TEST--
Test variations in usage of tan()
--INI--
serialize_precision = 10
--FILE--
<?php
/*
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
        null,
        true,
        false);

for ($i = 0; $i < count($values); $i++) {
    $res = tan($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(1.588153083)
float(-1.588153083)
float(8.895619796)
float(-8.895619796)
float(1.588153083)
float(1.588153083)
float(1.588153083)
float(8.895619796)
float(8.895619796)
float(1.470324156)
float(0)
float(1.557407725)
float(0)
