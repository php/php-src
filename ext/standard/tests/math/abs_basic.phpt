--TEST--
Test abs() function : basic functionality
--INI--
precision = 14
--FILE--
<?php
echo "*** Testing abs() : basic functionality ***\n";

$values = array(23,
                -23,
                2.345e1,
                -2.345e1,
                0x17,
                027,
                "23",
                "-23",
                "23.45",
                "2.345e1",
                "-2.345e1",
                null,
                true,
                false);

for ($i = 0; $i < count($values); $i++) {
    $res = abs($values[$i]);
    var_dump($res);
}
?>
--EXPECTF--
*** Testing abs() : basic functionality ***
int(23)
int(23)
float(23.45)
float(23.45)
int(23)
int(23)
int(23)
int(23)
float(23.45)
float(23.45)
float(23.45)

Deprecated: abs(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
int(0)
int(1)
int(0)
