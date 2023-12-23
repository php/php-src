--TEST--
Test pow() function : usage variations - different data types as $exp argument
--INI--
serialize_precision = 14
--FILE--
<?php
echo "*** Testing pow() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset($unset_var);

// heredoc string
$heredoc = <<<EOT
abc
xyz
EOT;

// get a class
class classA
{
}

// get a resource variable
$fp = fopen(__FILE__, "r");

$inputs = [
    // int data
    /*1*/ 0,
    1,
    12345,
    -2345,
    PHP_INT_MAX,

    // float data
    /*6*/ 2.5,
    -2.5,
    12.3456789e10,
    12.3456789e-10,
    0.5,

    // null data
    /*11*/ null,
    null,

    // boolean data
    /*13*/ true,
    false,
    true,
    false,

    // empty data
    /*17*/ "",
    "",
    [],

    // string data
    /*20*/ "abcxyz",
    "abcxyz",
    $heredoc,
    "5.5",
    "2",

    // object data
    /*25*/ new classA(),

    // undefined data
    /*26*/ @$undefined_var,

    // unset data
    /*27*/ @$unset_var,

    // resource variable
    /*28*/ $fp,
];

// loop through each element of $inputs to check the behaviour of pow()
$iterator = 1;
foreach ($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump(pow(20.3, $input));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
}
fclose($fp);
?>
--EXPECT--
*** Testing pow() : usage variations ***

-- Iteration 1 --
float(1)

-- Iteration 2 --
float(20.3)

-- Iteration 3 --
float(INF)

-- Iteration 4 --
float(0)

-- Iteration 5 --
float(INF)

-- Iteration 6 --
float(1856.6929774279)

-- Iteration 7 --
float(0.00053859200856424)

-- Iteration 8 --
float(INF)

-- Iteration 9 --
float(1.0000000037168)

-- Iteration 10 --
float(4.5055521304275)

-- Iteration 11 --
float(1)

-- Iteration 12 --
float(1)

-- Iteration 13 --
float(20.3)

-- Iteration 14 --
float(1)

-- Iteration 15 --
float(20.3)

-- Iteration 16 --
float(1)

-- Iteration 17 --
Unsupported operand types: float ** string

-- Iteration 18 --
Unsupported operand types: float ** string

-- Iteration 19 --
Unsupported operand types: float ** array

-- Iteration 20 --
Unsupported operand types: float ** string

-- Iteration 21 --
Unsupported operand types: float ** string

-- Iteration 22 --
Unsupported operand types: float ** string

-- Iteration 23 --
float(15532029.564086)

-- Iteration 24 --
float(412.09)

-- Iteration 25 --
Unsupported operand types: float ** classA

-- Iteration 26 --
float(1)

-- Iteration 27 --
float(1)

-- Iteration 28 --
Unsupported operand types: float ** resource
