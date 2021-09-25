--TEST--
Test log() - basic function test log()
--INI--
precision=14
--FILE--
<?php
$values = array(23,
                -23,
                2.345e1,
                -2.345e1,
                0x17,
                027,
                "23",
                "23.45",
                "2.345e1",
                true,
                false);

echo "\n LOG tests...no base\n";
for ($i = 0; $i < count($values); $i++) {
    $res = log($values[$i]);
    var_dump($res);
}

echo "\n LOG tests...base\n";
for ($i = 0; $i < count($values); $i++) {
    $res = log($values[$i], 4);
    var_dump($res);
}
?>
--EXPECT--
LOG tests...no base
float(3.1354942159291497)
float(NAN)
float(3.1548704948922883)
float(NAN)
float(3.1354942159291497)
float(3.1354942159291497)
float(3.1354942159291497)
float(3.1548704948922883)
float(3.1548704948922883)
float(0)
float(-INF)

 LOG tests...base
float(2.2617809780285065)
float(NAN)
float(2.275758008814007)
float(NAN)
float(2.2617809780285065)
float(2.2617809780285065)
float(2.2617809780285065)
float(2.275758008814007)
float(2.275758008814007)
float(0)
float(-INF)
