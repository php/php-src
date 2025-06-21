--TEST--
Test variations in usage of asinh()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = asinh($value);
    var_dump($res);
}

?>
--EXPECT--
float(3.8291136516208812)
float(-3.8291136516208812)
float(3.8484719917851553)
float(3.8484719917851553)
float(-3.8484719917851553)
float(2.99822295029797)
float(3.027639279353268)
float(8.97474462874175)
