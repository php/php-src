--TEST--
Test ceil() - basic function test for ceil()
--INI--
precision=14
--FILE--
<?php
echo "*** Testing ceil() : basic functionality ***\n";
$values = array(0,
                -0,
                0.0,
                -0.0,
                0.5,
                -0.5,
                1,
                -1,
                1.0,
                -1.0,
                1.5,
                -1.5,
                2.6,
                -2.6,
                037,
                0x5F,
                "10.5",
                "-10.5",
                "3.95E3",
                "-3.95E3",
                "039",
                true,
                false,
                null,
                );

for ($i = 0; $i < count($values); $i++) {
    $res = ceil($values[$i]);
    var_dump($res);
}

?>
--EXPECTF--
*** Testing ceil() : basic functionality ***
int(0)
int(0)
float(0)
float(-0)
float(1)
float(-0)
int(1)
int(-1)
float(1)
float(-1)
float(2)
float(-1)
float(3)
float(-2)
int(31)
int(95)
float(11)
float(-10)
float(3950)
float(-3950)
int(39)
int(1)
int(0)

Deprecated: ceil(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
int(0)
