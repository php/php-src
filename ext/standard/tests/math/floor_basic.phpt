--TEST--
Test floor() - basic function test for floor()
--INI--
precision=14
--FILE--
<?php
echo "*** Testing floor() : basic functionality ***\n";
$values = array(0,
                -0,
                0.5,
                -0.5,
                1,
                -1,
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

foreach($values as $value) {
    echo "\n-- floor $value --\n";
    var_dump(floor($value));
};

?>
--EXPECTF--
*** Testing floor() : basic functionality ***

-- floor 0 --
float(0)

-- floor 0 --
float(0)

-- floor 0.5 --
float(0)

-- floor -0.5 --
float(-1)

-- floor 1 --
float(1)

-- floor -1 --
float(-1)

-- floor 1.5 --
float(1)

-- floor -1.5 --
float(-2)

-- floor 2.6 --
float(2)

-- floor -2.6 --
float(-3)

-- floor 31 --
float(31)

-- floor 95 --
float(95)

-- floor 10.5 --
float(10)

-- floor -10.5 --
float(-11)

-- floor 3.95E3 --
float(3950)

-- floor -3.95E3 --
float(-3950)

-- floor 039 --
float(39)

-- floor 1 --
float(1)

-- floor  --
float(0)

-- floor  --

Deprecated: floor(): Passing null to parameter #1 ($num) of type int|float is deprecated in %s on line %d
float(0)
