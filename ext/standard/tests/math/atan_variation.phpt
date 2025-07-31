--TEST--
Test variations in usage of atan()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = atan($value);
    var_dump($res);
}

?>
--EXPECT--
float(1.5273454314033659)
float(-1.5273454314033659)
float(1.528178224770569)
float(1.528178224770569)
float(-1.528178224770569)
float(1.4711276743037347)
float(1.4740122816874226)
float(1.5705431942854318)
