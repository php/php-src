--TEST--
Test variations in usage of log10()
--INI--
precision=14
--FILE--
<?php
$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = log10($value);
    var_dump($res);
}

?>
--EXPECT--
float(1.3617278360175928)
float(NAN)
float(1.3701428470511021)
float(1.3701428470511021)
float(NAN)
float(1)
float(1.0128372247051722)
float(3.596652066132439)
