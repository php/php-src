--TEST--
Test variations in usage of tanh()
--INI--
precision=14
--FILE--
<?php
$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = tanh($value);
    var_dump($res);
}

?>
--EXPECT--
float(1)
float(-1)
float(1)
float(1)
float(-1)
float(0.9999999958776927)
float(0.9999999977376298)
float(1)
