--TEST--
Test variations in usage of atan()
--INI--
precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test atan with a different input values

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
    $res = atan($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(1.5273454314033659)
float(-1.5273454314033659)
float(1.528178224770569)
float(-1.528178224770569)
float(1.5273454314033659)
float(1.5273454314033659)
float(1.5273454314033659)
float(1.528178224770569)
float(1.528178224770569)
float(1.5697963271282298)
float(0.7853981633974483)
float(0)
