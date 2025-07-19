--TEST--
Test hypot() - basic function test hypot()
--INI--
precision=14
--FILE--
<?php

$values = include 'data/numbers.inc';

$xs = [
    33,
    -33,
    3.345e1,
    -3.345e1,
];

foreach ($values as $y) {
    foreach ($xs as $x) {
        echo "hypot(y: $y, x: $x): ";
        $res = hypot($y, $x);
        var_dump($res);
    }
}
?>
--EXPECT--
hypot(y: 23, x: 33): float(40.22437072223753)
hypot(y: 23, x: -33): float(40.22437072223753)
hypot(y: 23, x: 33.45): float(40.594365372548936)
hypot(y: 23, x: -33.45): float(40.594365372548936)
hypot(y: -23, x: 33): float(40.22437072223753)
hypot(y: -23, x: -33): float(40.22437072223753)
hypot(y: -23, x: 33.45): float(40.594365372548936)
hypot(y: -23, x: -33.45): float(40.594365372548936)
hypot(y: 23.45, x: 33): float(40.48336077946099)
hypot(y: 23.45, x: -33): float(40.48336077946099)
hypot(y: 23.45, x: 33.45): float(40.85100977944119)
hypot(y: 23.45, x: -33.45): float(40.85100977944119)
hypot(y: 23.45, x: 33): float(40.48336077946099)
hypot(y: 23.45, x: -33): float(40.48336077946099)
hypot(y: 23.45, x: 33.45): float(40.85100977944119)
hypot(y: 23.45, x: -33.45): float(40.85100977944119)
hypot(y: -23.45, x: 33): float(40.48336077946099)
hypot(y: -23.45, x: -33): float(40.48336077946099)
hypot(y: -23.45, x: 33.45): float(40.85100977944119)
hypot(y: -23.45, x: -33.45): float(40.85100977944119)
hypot(y: 10, x: 33): float(34.48187929913333)
hypot(y: 10, x: -33): float(34.48187929913333)
hypot(y: 10, x: 33.45): float(34.91278419146774)
hypot(y: 10, x: -33.45): float(34.91278419146774)
hypot(y: 10.3, x: 33): float(34.57007376329996)
hypot(y: 10.3, x: -33): float(34.57007376329996)
hypot(y: 10.3, x: 33.45): float(34.99989285697887)
hypot(y: 10.3, x: -33.45): float(34.99989285697887)
hypot(y: 3950.5, x: 33): float(3950.6378282500154)
hypot(y: 3950.5, x: -33): float(3950.6378282500154)
hypot(y: 3950.5, x: 33.45): float(3950.6416127636785)
hypot(y: 3950.5, x: -33.45): float(3950.6416127636785)
