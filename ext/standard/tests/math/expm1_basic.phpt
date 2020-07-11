--TEST--
Test expm1() - basic function test for expm1()
--INI--
serialize_precision=14
--FILE--
<?php
echo "*** Testing expm1() : basic functionality ***\n";
$values = array(10,
                10.3,
                3.9505e3,
                037,
                0x5F,
                "10",
                "3950.5",
                "3.9505e3",
                "039",
                true,
                false,
                null,
                );

// loop through each element of $values to check the behaviour of expm1()
$iterator = 1;
foreach($values as $value) {
    echo "\n-- Iteration $iterator --\n";
    var_dump(expm1($value));
    $iterator++;
};
?>
--EXPECT--
*** Testing expm1() : basic functionality ***

-- Iteration 1 --
float(22025.465794807)

-- Iteration 2 --
float(29731.618852891)

-- Iteration 3 --
float(INF)

-- Iteration 4 --
float(29048849665246)

-- Iteration 5 --
float(1.811239082889E+41)

-- Iteration 6 --
float(22025.465794807)

-- Iteration 7 --
float(INF)

-- Iteration 8 --
float(INF)

-- Iteration 9 --
float(8.6593400423994E+16)

-- Iteration 10 --
float(1.718281828459)

-- Iteration 11 --
float(0)

-- Iteration 12 --
float(0)
