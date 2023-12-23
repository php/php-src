--TEST--
Test pow() function : usage variations - different data types as $exp argument
--INI--
serialize_precision = 14
--FILE--
<?php
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
    0,
    1,
    12345,
    -2345,
    PHP_INT_MAX,

    // float data
    2.5,
    -2.5,
    12.3456789e10,
    12.3456789e-10,
    0.5,

    // null data
    null,

    // boolean data
    true,
    false,

    // empty data
    "",
    [],

    // string data
    "abcxyz",
    $heredoc,
    "5.5",
    "2",

    // object data
    new classA(),

    // resource variable
    $fp,
];

// loop through each element of $inputs to check the behaviour of pow()
foreach ($inputs as $input) {
    try {
        var_dump(pow(20.3, $input));
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
fclose($fp);
?>
--EXPECT--
float(1)
float(20.3)
float(INF)
float(0)
float(INF)
float(1856.6929774279)
float(0.00053859200856424)
float(INF)
float(1.0000000037168)
float(4.5055521304275)
float(1)
float(20.3)
float(1)
Unsupported operand types: float ** string
Unsupported operand types: float ** array
Unsupported operand types: float ** string
Unsupported operand types: float ** string
float(15532029.564086)
float(412.09)
Unsupported operand types: float ** classA
Unsupported operand types: float ** resource
