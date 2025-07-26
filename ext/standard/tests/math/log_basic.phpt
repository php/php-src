--TEST--
Test log() - basic function test log()
--INI--
precision=14
--FILE--
<?php
$values = include 'data/numbers.inc';

echo "LOG tests...no base\n";
foreach ($values as $value) {
    $res = log($value);
    var_dump($res);
}

echo "\nLOG tests...base 4\n";
foreach ($values as $value) {
    $res = log($value, 4);
    var_dump($res);
}
?>
--EXPECT--
LOG tests...no base
float(3.1354942159291497)
float(NAN)
float(3.1548704948922883)
float(3.1548704948922883)
float(NAN)
float(2.302585092994046)
float(2.33214389523559)
float(8.281597432162789)

LOG tests...base 4
float(2.2617809780285065)
float(NAN)
float(2.275758008814007)
float(2.275758008814007)
float(NAN)
float(1.6609640474436813)
float(1.682286216147928)
float(5.973909773010015)
