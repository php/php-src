--TEST--
Test variations in usage of cosh()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = cosh($value);
    var_dump($res);
}

?>
--EXPECT--
float(4872401723.124452)
float(4872401723.124452)
float(7641446994.979367)
float(7641446994.979367)
float(7641446994.979367)
float(11013.232920103324)
float(14866.309443262266)
float(INF)
