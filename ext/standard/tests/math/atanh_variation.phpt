--TEST--
Test variations in usage of atanh()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = atanh($value);
    var_dump($res);
}

?>
--EXPECT--
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
float(NAN)
