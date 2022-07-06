--TEST--
Test variations in usage of acosh()
--INI--
precision = 10
--FILE--
<?php
/*
 * Function is implemented in ext/standard/math.c
*/


//Test acosh with a different input values

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
    $res = acosh($values[$i]);
    var_dump($res);
}

?>
--EXPECT--
float(3.8281684713331012)
float(NAN)
float(3.8475627390640357)
float(NAN)
float(3.8281684713331012)
float(3.8281684713331012)
float(3.8281684713331012)
float(3.8475627390640357)
float(3.8475627390640357)
float(7.600902209541989)
float(NAN)
float(0)
float(NAN)
