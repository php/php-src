--TEST--
Test variations in usage of sinh()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = sinh($value);
    var_dump($res);
}

?>
--EXPECT--
float(4872401723.124452)
float(-4872401723.124452)
float(7641446994.979367)
float(7641446994.979367)
float(-7641446994.979367)
float(11013.232874703393)
float(14866.30940962917)
float(INF)
