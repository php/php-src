--TEST--
Test variations in usage of rad2deg()
--INI--
precision = 10
--FILE--
<?php

$values = include 'data/numbers.inc';

foreach ($values as $value) {
    $res = rad2deg($value);
    var_dump($res);
}

?>
--EXPECT--
float(1317.8029288008934)
float(-1317.8029288008934)
float(1343.5860295817804)
float(1343.5860295817804)
float(-1343.5860295817804)
float(572.9577951308232)
float(590.146528984748)
float(226346.97696643168)
