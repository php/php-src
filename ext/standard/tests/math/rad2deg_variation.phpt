--TEST--
Test variations in usage of rad2deg()
--INI--
precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test rad2deg with a different input values

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
    $res = rad2deg($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(1317.8029288008934)
float(-1317.8029288008934)
float(1343.5860295817804)
float(-1343.5860295817804)
float(1317.8029288008934)
float(1317.8029288008934)
float(1317.8029288008934)
float(1343.5860295817804)
float(1343.5860295817804)
float(57295.77951308232)
float(0)
float(57.29577951308232)
float(0)
