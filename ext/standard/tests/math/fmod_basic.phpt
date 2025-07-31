--TEST--
Test fmod() - basic function test fmod()
--INI--
precision=14
--FILE--
<?php
$values1 = [
    234,
    -234,
    23.45e1,
    -23.45e1,
];

$values2 = [
    2,
    -2,
    2.3e1,
    -2.3e1,
];

foreach ($values1 as $a) {
    foreach ($values2 as $b) {
        echo "fmod($a, $b): ";
        $res = fmod($a, $b);
        var_dump($res);
    }
}
?>
--EXPECT--
fmod(234, 2): float(0)
fmod(234, -2): float(0)
fmod(234, 23): float(4)
fmod(234, -23): float(4)
fmod(-234, 2): float(-0)
fmod(-234, -2): float(-0)
fmod(-234, 23): float(-4)
fmod(-234, -23): float(-4)
fmod(234.5, 2): float(0.5)
fmod(234.5, -2): float(0.5)
fmod(234.5, 23): float(4.5)
fmod(234.5, -23): float(4.5)
fmod(-234.5, 2): float(-0.5)
fmod(-234.5, -2): float(-0.5)
fmod(-234.5, 23): float(-4.5)
fmod(-234.5, -23): float(-4.5)
