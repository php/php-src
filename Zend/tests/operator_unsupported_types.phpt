--TEST--
Using unsupported types with operators
--FILE--
<?php

$binops = [
    '+',
    '-',
    '*',
    '/',
    '%',
    '**',
    '<<',
    '>>',
    '&',
    '|',
    '^',
    // Works on booleans, never errors.
    'xor',
    // Only generates errors that string conversion emits.
    '.',
];
$illegalValues = [
    '[]',
    'new stdClass',
    'STDOUT',
    '"foo"',
];
$legalValues = [
    'null',
    'true',
    'false',
    '2',
    '3.5', // Semi-legal for certain ops
    '"123"',
    '"123foo"', // Semi-legal
];

set_error_handler(function($errno, $errstr) {
    assert($errno == E_WARNING || $errno == E_DEPRECATED);
    echo "Warning: $errstr\n";
});

function evalBinOp(string $op, string $value1, string $value2) {
    try {
        eval("return $value1 $op $value2;");
        echo "No error for $value1 $op $value2\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

function evalAssignOp(string $op, string $value1, string $value2) {
    $x = $origX = eval("return $value1;");
    try {
        eval("\$x $op= $value2;");
        echo "No error for $value1 $op= $value2\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
        if ($x !== $origX) {
            die("Value corrupted!");
        }
    }
}

echo "BINARY OP:\n";
foreach ($binops as $op) {
    foreach ($illegalValues as $illegalValue1) {
        foreach ($illegalValues as $illegalValue2) {
            evalBinOp($op, $illegalValue1, $illegalValue2);
        }
    }
    foreach ($illegalValues as $illegalValue) {
        foreach ($legalValues as $legalValue) {
            evalBinOp($op, $illegalValue, $legalValue);
            evalBinOp($op, $legalValue, $illegalValue);
        }
    }
}

echo "\n\nASSIGN OP:\n";
foreach ($binops as $op) {
    if ($op === 'xor') continue;

    foreach ($illegalValues as $illegalValue1) {
        foreach ($illegalValues as $illegalValue2) {
            evalAssignOp($op, $illegalValue1, $illegalValue2);
        }
    }
    foreach ($illegalValues as $illegalValue) {
        foreach ($legalValues as $legalValue) {
            evalAssignOp($op, $illegalValue, $legalValue);
            evalAssignOp($op, $legalValue, $illegalValue);
        }
    }
}

echo "\n\nUNARY OP:\n";
foreach ($illegalValues as $illegalValue) {
    try {
        eval("return ~$illegalValue;");
        echo "No error for ~$illegalValue\n";
    } catch (TypeError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "\n\nINCDEC:\n";
foreach ($illegalValues as $illegalValue) {
    $copy = eval("return $illegalValue;");
    try {
        $copy++;
        echo "No error for $copy++\n";
    } catch (TypeError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    $copy = eval("return $illegalValue;");
    try {
        $copy--;
        echo "No error for $copy--\n";
    } catch (TypeError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
BINARY OP:
No error for [] + []
TypeError: Unsupported operand types: array + stdClass
TypeError: Unsupported operand types: array + resource
TypeError: Unsupported operand types: array + string
TypeError: Unsupported operand types: stdClass + array
TypeError: Unsupported operand types: stdClass + stdClass
TypeError: Unsupported operand types: stdClass + resource
TypeError: Unsupported operand types: stdClass + string
TypeError: Unsupported operand types: resource + array
TypeError: Unsupported operand types: resource + stdClass
TypeError: Unsupported operand types: resource + resource
TypeError: Unsupported operand types: resource + string
TypeError: Unsupported operand types: string + array
TypeError: Unsupported operand types: string + stdClass
TypeError: Unsupported operand types: string + resource
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: array + null
TypeError: Unsupported operand types: null + array
TypeError: Unsupported operand types: array + bool
TypeError: Unsupported operand types: bool + array
TypeError: Unsupported operand types: array + bool
TypeError: Unsupported operand types: bool + array
TypeError: Unsupported operand types: array + int
TypeError: Unsupported operand types: int + array
TypeError: Unsupported operand types: array + float
TypeError: Unsupported operand types: float + array
TypeError: Unsupported operand types: array + string
TypeError: Unsupported operand types: string + array
TypeError: Unsupported operand types: array + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + array
TypeError: Unsupported operand types: stdClass + null
TypeError: Unsupported operand types: null + stdClass
TypeError: Unsupported operand types: stdClass + bool
TypeError: Unsupported operand types: bool + stdClass
TypeError: Unsupported operand types: stdClass + bool
TypeError: Unsupported operand types: bool + stdClass
TypeError: Unsupported operand types: stdClass + int
TypeError: Unsupported operand types: int + stdClass
TypeError: Unsupported operand types: stdClass + float
TypeError: Unsupported operand types: float + stdClass
TypeError: Unsupported operand types: stdClass + string
TypeError: Unsupported operand types: string + stdClass
TypeError: Unsupported operand types: stdClass + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + stdClass
TypeError: Unsupported operand types: resource + null
TypeError: Unsupported operand types: null + resource
TypeError: Unsupported operand types: resource + bool
TypeError: Unsupported operand types: bool + resource
TypeError: Unsupported operand types: resource + bool
TypeError: Unsupported operand types: bool + resource
TypeError: Unsupported operand types: resource + int
TypeError: Unsupported operand types: int + resource
TypeError: Unsupported operand types: resource + float
TypeError: Unsupported operand types: float + resource
TypeError: Unsupported operand types: resource + string
TypeError: Unsupported operand types: string + resource
TypeError: Unsupported operand types: resource + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + resource
TypeError: Unsupported operand types: string + null
TypeError: Unsupported operand types: null + string
TypeError: Unsupported operand types: string + bool
TypeError: Unsupported operand types: bool + string
TypeError: Unsupported operand types: string + bool
TypeError: Unsupported operand types: bool + string
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: int + string
TypeError: Unsupported operand types: string + float
TypeError: Unsupported operand types: float + string
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: string + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: array - array
TypeError: Unsupported operand types: array - stdClass
TypeError: Unsupported operand types: array - resource
TypeError: Unsupported operand types: array - string
TypeError: Unsupported operand types: stdClass - array
TypeError: Unsupported operand types: stdClass - stdClass
TypeError: Unsupported operand types: stdClass - resource
TypeError: Unsupported operand types: stdClass - string
TypeError: Unsupported operand types: resource - array
TypeError: Unsupported operand types: resource - stdClass
TypeError: Unsupported operand types: resource - resource
TypeError: Unsupported operand types: resource - string
TypeError: Unsupported operand types: string - array
TypeError: Unsupported operand types: string - stdClass
TypeError: Unsupported operand types: string - resource
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: array - null
TypeError: Unsupported operand types: null - array
TypeError: Unsupported operand types: array - bool
TypeError: Unsupported operand types: bool - array
TypeError: Unsupported operand types: array - bool
TypeError: Unsupported operand types: bool - array
TypeError: Unsupported operand types: array - int
TypeError: Unsupported operand types: int - array
TypeError: Unsupported operand types: array - float
TypeError: Unsupported operand types: float - array
TypeError: Unsupported operand types: array - string
TypeError: Unsupported operand types: string - array
TypeError: Unsupported operand types: array - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - array
TypeError: Unsupported operand types: stdClass - null
TypeError: Unsupported operand types: null - stdClass
TypeError: Unsupported operand types: stdClass - bool
TypeError: Unsupported operand types: bool - stdClass
TypeError: Unsupported operand types: stdClass - bool
TypeError: Unsupported operand types: bool - stdClass
TypeError: Unsupported operand types: stdClass - int
TypeError: Unsupported operand types: int - stdClass
TypeError: Unsupported operand types: stdClass - float
TypeError: Unsupported operand types: float - stdClass
TypeError: Unsupported operand types: stdClass - string
TypeError: Unsupported operand types: string - stdClass
TypeError: Unsupported operand types: stdClass - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - stdClass
TypeError: Unsupported operand types: resource - null
TypeError: Unsupported operand types: null - resource
TypeError: Unsupported operand types: resource - bool
TypeError: Unsupported operand types: bool - resource
TypeError: Unsupported operand types: resource - bool
TypeError: Unsupported operand types: bool - resource
TypeError: Unsupported operand types: resource - int
TypeError: Unsupported operand types: int - resource
TypeError: Unsupported operand types: resource - float
TypeError: Unsupported operand types: float - resource
TypeError: Unsupported operand types: resource - string
TypeError: Unsupported operand types: string - resource
TypeError: Unsupported operand types: resource - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - resource
TypeError: Unsupported operand types: string - null
TypeError: Unsupported operand types: null - string
TypeError: Unsupported operand types: string - bool
TypeError: Unsupported operand types: bool - string
TypeError: Unsupported operand types: string - bool
TypeError: Unsupported operand types: bool - string
TypeError: Unsupported operand types: string - int
TypeError: Unsupported operand types: int - string
TypeError: Unsupported operand types: string - float
TypeError: Unsupported operand types: float - string
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: string - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: array * array
TypeError: Unsupported operand types: stdClass * array
TypeError: Unsupported operand types: resource * array
TypeError: Unsupported operand types: array * string
TypeError: Unsupported operand types: stdClass * array
TypeError: Unsupported operand types: stdClass * stdClass
TypeError: Unsupported operand types: stdClass * resource
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: resource * array
TypeError: Unsupported operand types: resource * stdClass
TypeError: Unsupported operand types: resource * resource
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: string * array
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: array * null
TypeError: Unsupported operand types: null * array
TypeError: Unsupported operand types: array * bool
TypeError: Unsupported operand types: bool * array
TypeError: Unsupported operand types: array * bool
TypeError: Unsupported operand types: bool * array
TypeError: Unsupported operand types: array * int
TypeError: Unsupported operand types: int * array
TypeError: Unsupported operand types: array * float
TypeError: Unsupported operand types: float * array
TypeError: Unsupported operand types: array * string
TypeError: Unsupported operand types: string * array
TypeError: Unsupported operand types: array * string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string * array
TypeError: Unsupported operand types: stdClass * null
TypeError: Unsupported operand types: stdClass * null
TypeError: Unsupported operand types: stdClass * bool
TypeError: Unsupported operand types: stdClass * bool
TypeError: Unsupported operand types: stdClass * bool
TypeError: Unsupported operand types: stdClass * bool
TypeError: Unsupported operand types: stdClass * int
TypeError: Unsupported operand types: stdClass * int
TypeError: Unsupported operand types: stdClass * float
TypeError: Unsupported operand types: stdClass * float
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: resource * null
TypeError: Unsupported operand types: resource * null
TypeError: Unsupported operand types: resource * bool
TypeError: Unsupported operand types: resource * bool
TypeError: Unsupported operand types: resource * bool
TypeError: Unsupported operand types: resource * bool
TypeError: Unsupported operand types: resource * int
TypeError: Unsupported operand types: resource * int
TypeError: Unsupported operand types: resource * float
TypeError: Unsupported operand types: resource * float
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: string * null
TypeError: Unsupported operand types: null * string
TypeError: Unsupported operand types: string * bool
TypeError: Unsupported operand types: bool * string
TypeError: Unsupported operand types: string * bool
TypeError: Unsupported operand types: bool * string
TypeError: Unsupported operand types: string * int
TypeError: Unsupported operand types: int * string
TypeError: Unsupported operand types: string * float
TypeError: Unsupported operand types: float * string
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: string * string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: array / array
TypeError: Unsupported operand types: array / stdClass
TypeError: Unsupported operand types: array / resource
TypeError: Unsupported operand types: array / string
TypeError: Unsupported operand types: stdClass / array
TypeError: Unsupported operand types: stdClass / stdClass
TypeError: Unsupported operand types: stdClass / resource
TypeError: Unsupported operand types: stdClass / string
TypeError: Unsupported operand types: resource / array
TypeError: Unsupported operand types: resource / stdClass
TypeError: Unsupported operand types: resource / resource
TypeError: Unsupported operand types: resource / string
TypeError: Unsupported operand types: string / array
TypeError: Unsupported operand types: string / stdClass
TypeError: Unsupported operand types: string / resource
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: array / null
TypeError: Unsupported operand types: null / array
TypeError: Unsupported operand types: array / bool
TypeError: Unsupported operand types: bool / array
TypeError: Unsupported operand types: array / bool
TypeError: Unsupported operand types: bool / array
TypeError: Unsupported operand types: array / int
TypeError: Unsupported operand types: int / array
TypeError: Unsupported operand types: array / float
TypeError: Unsupported operand types: float / array
TypeError: Unsupported operand types: array / string
TypeError: Unsupported operand types: string / array
TypeError: Unsupported operand types: array / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / array
TypeError: Unsupported operand types: stdClass / null
TypeError: Unsupported operand types: null / stdClass
TypeError: Unsupported operand types: stdClass / bool
TypeError: Unsupported operand types: bool / stdClass
TypeError: Unsupported operand types: stdClass / bool
TypeError: Unsupported operand types: bool / stdClass
TypeError: Unsupported operand types: stdClass / int
TypeError: Unsupported operand types: int / stdClass
TypeError: Unsupported operand types: stdClass / float
TypeError: Unsupported operand types: float / stdClass
TypeError: Unsupported operand types: stdClass / string
TypeError: Unsupported operand types: string / stdClass
TypeError: Unsupported operand types: stdClass / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / stdClass
TypeError: Unsupported operand types: resource / null
TypeError: Unsupported operand types: null / resource
TypeError: Unsupported operand types: resource / bool
TypeError: Unsupported operand types: bool / resource
TypeError: Unsupported operand types: resource / bool
TypeError: Unsupported operand types: bool / resource
TypeError: Unsupported operand types: resource / int
TypeError: Unsupported operand types: int / resource
TypeError: Unsupported operand types: resource / float
TypeError: Unsupported operand types: float / resource
TypeError: Unsupported operand types: resource / string
TypeError: Unsupported operand types: string / resource
TypeError: Unsupported operand types: resource / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / resource
TypeError: Unsupported operand types: string / null
TypeError: Unsupported operand types: null / string
TypeError: Unsupported operand types: string / bool
TypeError: Unsupported operand types: bool / string
TypeError: Unsupported operand types: string / bool
TypeError: Unsupported operand types: bool / string
TypeError: Unsupported operand types: string / int
TypeError: Unsupported operand types: int / string
TypeError: Unsupported operand types: string / float
TypeError: Unsupported operand types: float / string
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: string / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: array % array
TypeError: Unsupported operand types: array % stdClass
TypeError: Unsupported operand types: array % resource
TypeError: Unsupported operand types: array % string
TypeError: Unsupported operand types: stdClass % array
TypeError: Unsupported operand types: stdClass % stdClass
TypeError: Unsupported operand types: stdClass % resource
TypeError: Unsupported operand types: stdClass % string
TypeError: Unsupported operand types: resource % array
TypeError: Unsupported operand types: resource % stdClass
TypeError: Unsupported operand types: resource % resource
TypeError: Unsupported operand types: resource % string
TypeError: Unsupported operand types: string % array
TypeError: Unsupported operand types: string % stdClass
TypeError: Unsupported operand types: string % resource
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: array % null
TypeError: Unsupported operand types: null % array
TypeError: Unsupported operand types: array % bool
TypeError: Unsupported operand types: bool % array
TypeError: Unsupported operand types: array % bool
TypeError: Unsupported operand types: bool % array
TypeError: Unsupported operand types: array % int
TypeError: Unsupported operand types: int % array
TypeError: Unsupported operand types: array % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % array
TypeError: Unsupported operand types: array % string
TypeError: Unsupported operand types: string % array
TypeError: Unsupported operand types: array % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % array
TypeError: Unsupported operand types: stdClass % null
TypeError: Unsupported operand types: null % stdClass
TypeError: Unsupported operand types: stdClass % bool
TypeError: Unsupported operand types: bool % stdClass
TypeError: Unsupported operand types: stdClass % bool
TypeError: Unsupported operand types: bool % stdClass
TypeError: Unsupported operand types: stdClass % int
TypeError: Unsupported operand types: int % stdClass
TypeError: Unsupported operand types: stdClass % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % stdClass
TypeError: Unsupported operand types: stdClass % string
TypeError: Unsupported operand types: string % stdClass
TypeError: Unsupported operand types: stdClass % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % stdClass
TypeError: Unsupported operand types: resource % null
TypeError: Unsupported operand types: null % resource
TypeError: Unsupported operand types: resource % bool
TypeError: Unsupported operand types: bool % resource
TypeError: Unsupported operand types: resource % bool
TypeError: Unsupported operand types: bool % resource
TypeError: Unsupported operand types: resource % int
TypeError: Unsupported operand types: int % resource
TypeError: Unsupported operand types: resource % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % resource
TypeError: Unsupported operand types: resource % string
TypeError: Unsupported operand types: string % resource
TypeError: Unsupported operand types: resource % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % resource
TypeError: Unsupported operand types: string % null
TypeError: Unsupported operand types: null % string
TypeError: Unsupported operand types: string % bool
TypeError: Unsupported operand types: bool % string
TypeError: Unsupported operand types: string % bool
TypeError: Unsupported operand types: bool % string
TypeError: Unsupported operand types: string % int
TypeError: Unsupported operand types: int % string
TypeError: Unsupported operand types: string % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % string
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: string % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: array ** array
TypeError: Unsupported operand types: array ** stdClass
TypeError: Unsupported operand types: array ** resource
TypeError: Unsupported operand types: array ** string
TypeError: Unsupported operand types: stdClass ** array
TypeError: Unsupported operand types: stdClass ** stdClass
TypeError: Unsupported operand types: stdClass ** resource
TypeError: Unsupported operand types: stdClass ** string
TypeError: Unsupported operand types: resource ** array
TypeError: Unsupported operand types: resource ** stdClass
TypeError: Unsupported operand types: resource ** resource
TypeError: Unsupported operand types: resource ** string
TypeError: Unsupported operand types: string ** array
TypeError: Unsupported operand types: string ** stdClass
TypeError: Unsupported operand types: string ** resource
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: array ** null
TypeError: Unsupported operand types: null ** array
TypeError: Unsupported operand types: array ** bool
TypeError: Unsupported operand types: bool ** array
TypeError: Unsupported operand types: array ** bool
TypeError: Unsupported operand types: bool ** array
TypeError: Unsupported operand types: array ** int
TypeError: Unsupported operand types: int ** array
TypeError: Unsupported operand types: array ** float
TypeError: Unsupported operand types: float ** array
TypeError: Unsupported operand types: array ** string
TypeError: Unsupported operand types: string ** array
TypeError: Unsupported operand types: array ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** array
TypeError: Unsupported operand types: stdClass ** null
TypeError: Unsupported operand types: null ** stdClass
TypeError: Unsupported operand types: stdClass ** bool
TypeError: Unsupported operand types: bool ** stdClass
TypeError: Unsupported operand types: stdClass ** bool
TypeError: Unsupported operand types: bool ** stdClass
TypeError: Unsupported operand types: stdClass ** int
TypeError: Unsupported operand types: int ** stdClass
TypeError: Unsupported operand types: stdClass ** float
TypeError: Unsupported operand types: float ** stdClass
TypeError: Unsupported operand types: stdClass ** string
TypeError: Unsupported operand types: string ** stdClass
TypeError: Unsupported operand types: stdClass ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** stdClass
TypeError: Unsupported operand types: resource ** null
TypeError: Unsupported operand types: null ** resource
TypeError: Unsupported operand types: resource ** bool
TypeError: Unsupported operand types: bool ** resource
TypeError: Unsupported operand types: resource ** bool
TypeError: Unsupported operand types: bool ** resource
TypeError: Unsupported operand types: resource ** int
TypeError: Unsupported operand types: int ** resource
TypeError: Unsupported operand types: resource ** float
TypeError: Unsupported operand types: float ** resource
TypeError: Unsupported operand types: resource ** string
TypeError: Unsupported operand types: string ** resource
TypeError: Unsupported operand types: resource ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** resource
TypeError: Unsupported operand types: string ** null
TypeError: Unsupported operand types: null ** string
TypeError: Unsupported operand types: string ** bool
TypeError: Unsupported operand types: bool ** string
TypeError: Unsupported operand types: string ** bool
TypeError: Unsupported operand types: bool ** string
TypeError: Unsupported operand types: string ** int
TypeError: Unsupported operand types: int ** string
TypeError: Unsupported operand types: string ** float
TypeError: Unsupported operand types: float ** string
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: string ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: array << array
TypeError: Unsupported operand types: array << stdClass
TypeError: Unsupported operand types: array << resource
TypeError: Unsupported operand types: array << string
TypeError: Unsupported operand types: stdClass << array
TypeError: Unsupported operand types: stdClass << stdClass
TypeError: Unsupported operand types: stdClass << resource
TypeError: Unsupported operand types: stdClass << string
TypeError: Unsupported operand types: resource << array
TypeError: Unsupported operand types: resource << stdClass
TypeError: Unsupported operand types: resource << resource
TypeError: Unsupported operand types: resource << string
TypeError: Unsupported operand types: string << array
TypeError: Unsupported operand types: string << stdClass
TypeError: Unsupported operand types: string << resource
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: array << null
TypeError: Unsupported operand types: null << array
TypeError: Unsupported operand types: array << bool
TypeError: Unsupported operand types: bool << array
TypeError: Unsupported operand types: array << bool
TypeError: Unsupported operand types: bool << array
TypeError: Unsupported operand types: array << int
TypeError: Unsupported operand types: int << array
TypeError: Unsupported operand types: array << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << array
TypeError: Unsupported operand types: array << string
TypeError: Unsupported operand types: string << array
TypeError: Unsupported operand types: array << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << array
TypeError: Unsupported operand types: stdClass << null
TypeError: Unsupported operand types: null << stdClass
TypeError: Unsupported operand types: stdClass << bool
TypeError: Unsupported operand types: bool << stdClass
TypeError: Unsupported operand types: stdClass << bool
TypeError: Unsupported operand types: bool << stdClass
TypeError: Unsupported operand types: stdClass << int
TypeError: Unsupported operand types: int << stdClass
TypeError: Unsupported operand types: stdClass << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << stdClass
TypeError: Unsupported operand types: stdClass << string
TypeError: Unsupported operand types: string << stdClass
TypeError: Unsupported operand types: stdClass << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << stdClass
TypeError: Unsupported operand types: resource << null
TypeError: Unsupported operand types: null << resource
TypeError: Unsupported operand types: resource << bool
TypeError: Unsupported operand types: bool << resource
TypeError: Unsupported operand types: resource << bool
TypeError: Unsupported operand types: bool << resource
TypeError: Unsupported operand types: resource << int
TypeError: Unsupported operand types: int << resource
TypeError: Unsupported operand types: resource << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << resource
TypeError: Unsupported operand types: resource << string
TypeError: Unsupported operand types: string << resource
TypeError: Unsupported operand types: resource << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << resource
TypeError: Unsupported operand types: string << null
TypeError: Unsupported operand types: null << string
TypeError: Unsupported operand types: string << bool
TypeError: Unsupported operand types: bool << string
TypeError: Unsupported operand types: string << bool
TypeError: Unsupported operand types: bool << string
TypeError: Unsupported operand types: string << int
TypeError: Unsupported operand types: int << string
TypeError: Unsupported operand types: string << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << string
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: string << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: array >> array
TypeError: Unsupported operand types: array >> stdClass
TypeError: Unsupported operand types: array >> resource
TypeError: Unsupported operand types: array >> string
TypeError: Unsupported operand types: stdClass >> array
TypeError: Unsupported operand types: stdClass >> stdClass
TypeError: Unsupported operand types: stdClass >> resource
TypeError: Unsupported operand types: stdClass >> string
TypeError: Unsupported operand types: resource >> array
TypeError: Unsupported operand types: resource >> stdClass
TypeError: Unsupported operand types: resource >> resource
TypeError: Unsupported operand types: resource >> string
TypeError: Unsupported operand types: string >> array
TypeError: Unsupported operand types: string >> stdClass
TypeError: Unsupported operand types: string >> resource
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: array >> null
TypeError: Unsupported operand types: null >> array
TypeError: Unsupported operand types: array >> bool
TypeError: Unsupported operand types: bool >> array
TypeError: Unsupported operand types: array >> bool
TypeError: Unsupported operand types: bool >> array
TypeError: Unsupported operand types: array >> int
TypeError: Unsupported operand types: int >> array
TypeError: Unsupported operand types: array >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> array
TypeError: Unsupported operand types: array >> string
TypeError: Unsupported operand types: string >> array
TypeError: Unsupported operand types: array >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> array
TypeError: Unsupported operand types: stdClass >> null
TypeError: Unsupported operand types: null >> stdClass
TypeError: Unsupported operand types: stdClass >> bool
TypeError: Unsupported operand types: bool >> stdClass
TypeError: Unsupported operand types: stdClass >> bool
TypeError: Unsupported operand types: bool >> stdClass
TypeError: Unsupported operand types: stdClass >> int
TypeError: Unsupported operand types: int >> stdClass
TypeError: Unsupported operand types: stdClass >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> stdClass
TypeError: Unsupported operand types: stdClass >> string
TypeError: Unsupported operand types: string >> stdClass
TypeError: Unsupported operand types: stdClass >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> stdClass
TypeError: Unsupported operand types: resource >> null
TypeError: Unsupported operand types: null >> resource
TypeError: Unsupported operand types: resource >> bool
TypeError: Unsupported operand types: bool >> resource
TypeError: Unsupported operand types: resource >> bool
TypeError: Unsupported operand types: bool >> resource
TypeError: Unsupported operand types: resource >> int
TypeError: Unsupported operand types: int >> resource
TypeError: Unsupported operand types: resource >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> resource
TypeError: Unsupported operand types: resource >> string
TypeError: Unsupported operand types: string >> resource
TypeError: Unsupported operand types: resource >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> resource
TypeError: Unsupported operand types: string >> null
TypeError: Unsupported operand types: null >> string
TypeError: Unsupported operand types: string >> bool
TypeError: Unsupported operand types: bool >> string
TypeError: Unsupported operand types: string >> bool
TypeError: Unsupported operand types: bool >> string
TypeError: Unsupported operand types: string >> int
TypeError: Unsupported operand types: int >> string
TypeError: Unsupported operand types: string >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> string
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: string >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: array & array
TypeError: Unsupported operand types: stdClass & array
TypeError: Unsupported operand types: resource & array
TypeError: Unsupported operand types: array & string
TypeError: Unsupported operand types: stdClass & array
TypeError: Unsupported operand types: stdClass & stdClass
TypeError: Unsupported operand types: stdClass & resource
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: resource & array
TypeError: Unsupported operand types: resource & stdClass
TypeError: Unsupported operand types: resource & resource
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: string & array
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: resource & string
No error for "foo" & "foo"
TypeError: Unsupported operand types: array & null
TypeError: Unsupported operand types: null & array
TypeError: Unsupported operand types: array & bool
TypeError: Unsupported operand types: bool & array
TypeError: Unsupported operand types: array & bool
TypeError: Unsupported operand types: bool & array
TypeError: Unsupported operand types: array & int
TypeError: Unsupported operand types: int & array
TypeError: Unsupported operand types: array & float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float & array
TypeError: Unsupported operand types: array & string
TypeError: Unsupported operand types: string & array
TypeError: Unsupported operand types: array & string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string & array
TypeError: Unsupported operand types: stdClass & null
TypeError: Unsupported operand types: stdClass & null
TypeError: Unsupported operand types: stdClass & bool
TypeError: Unsupported operand types: stdClass & bool
TypeError: Unsupported operand types: stdClass & bool
TypeError: Unsupported operand types: stdClass & bool
TypeError: Unsupported operand types: stdClass & int
TypeError: Unsupported operand types: stdClass & int
TypeError: Unsupported operand types: stdClass & float
TypeError: Unsupported operand types: stdClass & float
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: resource & null
TypeError: Unsupported operand types: resource & null
TypeError: Unsupported operand types: resource & bool
TypeError: Unsupported operand types: resource & bool
TypeError: Unsupported operand types: resource & bool
TypeError: Unsupported operand types: resource & bool
TypeError: Unsupported operand types: resource & int
TypeError: Unsupported operand types: resource & int
TypeError: Unsupported operand types: resource & float
TypeError: Unsupported operand types: resource & float
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: string & null
TypeError: Unsupported operand types: null & string
TypeError: Unsupported operand types: string & bool
TypeError: Unsupported operand types: bool & string
TypeError: Unsupported operand types: string & bool
TypeError: Unsupported operand types: bool & string
TypeError: Unsupported operand types: string & int
TypeError: Unsupported operand types: int & string
TypeError: Unsupported operand types: string & float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float & string
No error for "foo" & "123"
No error for "123" & "foo"
No error for "foo" & "123foo"
No error for "123foo" & "foo"
TypeError: Unsupported operand types: array | array
TypeError: Unsupported operand types: stdClass | array
TypeError: Unsupported operand types: resource | array
TypeError: Unsupported operand types: array | string
TypeError: Unsupported operand types: stdClass | array
TypeError: Unsupported operand types: stdClass | stdClass
TypeError: Unsupported operand types: stdClass | resource
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: resource | array
TypeError: Unsupported operand types: resource | stdClass
TypeError: Unsupported operand types: resource | resource
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: string | array
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: resource | string
No error for "foo" | "foo"
TypeError: Unsupported operand types: array | null
TypeError: Unsupported operand types: null | array
TypeError: Unsupported operand types: array | bool
TypeError: Unsupported operand types: bool | array
TypeError: Unsupported operand types: array | bool
TypeError: Unsupported operand types: bool | array
TypeError: Unsupported operand types: array | int
TypeError: Unsupported operand types: int | array
TypeError: Unsupported operand types: array | float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float | array
TypeError: Unsupported operand types: array | string
TypeError: Unsupported operand types: string | array
TypeError: Unsupported operand types: array | string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string | array
TypeError: Unsupported operand types: stdClass | null
TypeError: Unsupported operand types: stdClass | null
TypeError: Unsupported operand types: stdClass | bool
TypeError: Unsupported operand types: stdClass | bool
TypeError: Unsupported operand types: stdClass | bool
TypeError: Unsupported operand types: stdClass | bool
TypeError: Unsupported operand types: stdClass | int
TypeError: Unsupported operand types: stdClass | int
TypeError: Unsupported operand types: stdClass | float
TypeError: Unsupported operand types: stdClass | float
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: resource | null
TypeError: Unsupported operand types: resource | null
TypeError: Unsupported operand types: resource | bool
TypeError: Unsupported operand types: resource | bool
TypeError: Unsupported operand types: resource | bool
TypeError: Unsupported operand types: resource | bool
TypeError: Unsupported operand types: resource | int
TypeError: Unsupported operand types: resource | int
TypeError: Unsupported operand types: resource | float
TypeError: Unsupported operand types: resource | float
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: string | null
TypeError: Unsupported operand types: null | string
TypeError: Unsupported operand types: string | bool
TypeError: Unsupported operand types: bool | string
TypeError: Unsupported operand types: string | bool
TypeError: Unsupported operand types: bool | string
TypeError: Unsupported operand types: string | int
TypeError: Unsupported operand types: int | string
TypeError: Unsupported operand types: string | float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float | string
No error for "foo" | "123"
No error for "123" | "foo"
No error for "foo" | "123foo"
No error for "123foo" | "foo"
TypeError: Unsupported operand types: array ^ array
TypeError: Unsupported operand types: stdClass ^ array
TypeError: Unsupported operand types: resource ^ array
TypeError: Unsupported operand types: array ^ string
TypeError: Unsupported operand types: stdClass ^ array
TypeError: Unsupported operand types: stdClass ^ stdClass
TypeError: Unsupported operand types: stdClass ^ resource
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: resource ^ array
TypeError: Unsupported operand types: resource ^ stdClass
TypeError: Unsupported operand types: resource ^ resource
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: string ^ array
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: resource ^ string
No error for "foo" ^ "foo"
TypeError: Unsupported operand types: array ^ null
TypeError: Unsupported operand types: null ^ array
TypeError: Unsupported operand types: array ^ bool
TypeError: Unsupported operand types: bool ^ array
TypeError: Unsupported operand types: array ^ bool
TypeError: Unsupported operand types: bool ^ array
TypeError: Unsupported operand types: array ^ int
TypeError: Unsupported operand types: int ^ array
TypeError: Unsupported operand types: array ^ float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float ^ array
TypeError: Unsupported operand types: array ^ string
TypeError: Unsupported operand types: string ^ array
TypeError: Unsupported operand types: array ^ string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ^ array
TypeError: Unsupported operand types: stdClass ^ null
TypeError: Unsupported operand types: stdClass ^ null
TypeError: Unsupported operand types: stdClass ^ bool
TypeError: Unsupported operand types: stdClass ^ bool
TypeError: Unsupported operand types: stdClass ^ bool
TypeError: Unsupported operand types: stdClass ^ bool
TypeError: Unsupported operand types: stdClass ^ int
TypeError: Unsupported operand types: stdClass ^ int
TypeError: Unsupported operand types: stdClass ^ float
TypeError: Unsupported operand types: stdClass ^ float
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: resource ^ null
TypeError: Unsupported operand types: resource ^ null
TypeError: Unsupported operand types: resource ^ bool
TypeError: Unsupported operand types: resource ^ bool
TypeError: Unsupported operand types: resource ^ bool
TypeError: Unsupported operand types: resource ^ bool
TypeError: Unsupported operand types: resource ^ int
TypeError: Unsupported operand types: resource ^ int
TypeError: Unsupported operand types: resource ^ float
TypeError: Unsupported operand types: resource ^ float
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: string ^ null
TypeError: Unsupported operand types: null ^ string
TypeError: Unsupported operand types: string ^ bool
TypeError: Unsupported operand types: bool ^ string
TypeError: Unsupported operand types: string ^ bool
TypeError: Unsupported operand types: bool ^ string
TypeError: Unsupported operand types: string ^ int
TypeError: Unsupported operand types: int ^ string
TypeError: Unsupported operand types: string ^ float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float ^ string
No error for "foo" ^ "123"
No error for "123" ^ "foo"
No error for "foo" ^ "123foo"
No error for "123foo" ^ "foo"
No error for [] xor []
No error for [] xor new stdClass
No error for [] xor STDOUT
No error for [] xor "foo"
No error for new stdClass xor []
No error for new stdClass xor new stdClass
No error for new stdClass xor STDOUT
No error for new stdClass xor "foo"
No error for STDOUT xor []
No error for STDOUT xor new stdClass
No error for STDOUT xor STDOUT
No error for STDOUT xor "foo"
No error for "foo" xor []
No error for "foo" xor new stdClass
No error for "foo" xor STDOUT
No error for "foo" xor "foo"
No error for [] xor null
No error for null xor []
No error for [] xor true
No error for true xor []
No error for [] xor false
No error for false xor []
No error for [] xor 2
No error for 2 xor []
No error for [] xor 3.5
No error for 3.5 xor []
No error for [] xor "123"
No error for "123" xor []
No error for [] xor "123foo"
No error for "123foo" xor []
No error for new stdClass xor null
No error for null xor new stdClass
No error for new stdClass xor true
No error for true xor new stdClass
No error for new stdClass xor false
No error for false xor new stdClass
No error for new stdClass xor 2
No error for 2 xor new stdClass
No error for new stdClass xor 3.5
No error for 3.5 xor new stdClass
No error for new stdClass xor "123"
No error for "123" xor new stdClass
No error for new stdClass xor "123foo"
No error for "123foo" xor new stdClass
No error for STDOUT xor null
No error for null xor STDOUT
No error for STDOUT xor true
No error for true xor STDOUT
No error for STDOUT xor false
No error for false xor STDOUT
No error for STDOUT xor 2
No error for 2 xor STDOUT
No error for STDOUT xor 3.5
No error for 3.5 xor STDOUT
No error for STDOUT xor "123"
No error for "123" xor STDOUT
No error for STDOUT xor "123foo"
No error for "123foo" xor STDOUT
No error for "foo" xor null
No error for null xor "foo"
No error for "foo" xor true
No error for true xor "foo"
No error for "foo" xor false
No error for false xor "foo"
No error for "foo" xor 2
No error for 2 xor "foo"
No error for "foo" xor 3.5
No error for 3.5 xor "foo"
No error for "foo" xor "123"
No error for "123" xor "foo"
No error for "foo" xor "123foo"
No error for "123foo" xor "foo"
Warning: Array to string conversion
Warning: Array to string conversion
No error for [] . []
Warning: Array to string conversion
Error: Object of class stdClass could not be converted to string
Warning: Array to string conversion
No error for [] . STDOUT
Warning: Array to string conversion
No error for [] . "foo"
Warning: Array to string conversion
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Warning: Array to string conversion
No error for STDOUT . []
Error: Object of class stdClass could not be converted to string
No error for STDOUT . STDOUT
No error for STDOUT . "foo"
Warning: Array to string conversion
No error for "foo" . []
Error: Object of class stdClass could not be converted to string
No error for "foo" . STDOUT
No error for "foo" . "foo"
Warning: Array to string conversion
No error for [] . null
Warning: Array to string conversion
No error for null . []
Warning: Array to string conversion
No error for [] . true
Warning: Array to string conversion
No error for true . []
Warning: Array to string conversion
No error for [] . false
Warning: Array to string conversion
No error for false . []
Warning: Array to string conversion
No error for [] . 2
Warning: Array to string conversion
No error for 2 . []
Warning: Array to string conversion
No error for [] . 3.5
Warning: Array to string conversion
No error for 3.5 . []
Warning: Array to string conversion
No error for [] . "123"
Warning: Array to string conversion
No error for "123" . []
Warning: Array to string conversion
No error for [] . "123foo"
Warning: Array to string conversion
No error for "123foo" . []
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
No error for STDOUT . null
No error for null . STDOUT
No error for STDOUT . true
No error for true . STDOUT
No error for STDOUT . false
No error for false . STDOUT
No error for STDOUT . 2
No error for 2 . STDOUT
No error for STDOUT . 3.5
No error for 3.5 . STDOUT
No error for STDOUT . "123"
No error for "123" . STDOUT
No error for STDOUT . "123foo"
No error for "123foo" . STDOUT
No error for "foo" . null
No error for null . "foo"
No error for "foo" . true
No error for true . "foo"
No error for "foo" . false
No error for false . "foo"
No error for "foo" . 2
No error for 2 . "foo"
No error for "foo" . 3.5
No error for 3.5 . "foo"
No error for "foo" . "123"
No error for "123" . "foo"
No error for "foo" . "123foo"
No error for "123foo" . "foo"


ASSIGN OP:
No error for [] += []
TypeError: Unsupported operand types: array + stdClass
TypeError: Unsupported operand types: array + resource
TypeError: Unsupported operand types: array + string
TypeError: Unsupported operand types: stdClass + array
TypeError: Unsupported operand types: stdClass + stdClass
TypeError: Unsupported operand types: stdClass + resource
TypeError: Unsupported operand types: stdClass + string
TypeError: Unsupported operand types: resource + array
TypeError: Unsupported operand types: resource + stdClass
TypeError: Unsupported operand types: resource + resource
TypeError: Unsupported operand types: resource + string
TypeError: Unsupported operand types: string + array
TypeError: Unsupported operand types: string + stdClass
TypeError: Unsupported operand types: string + resource
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: array + null
TypeError: Unsupported operand types: null + array
TypeError: Unsupported operand types: array + bool
TypeError: Unsupported operand types: bool + array
TypeError: Unsupported operand types: array + bool
TypeError: Unsupported operand types: bool + array
TypeError: Unsupported operand types: array + int
TypeError: Unsupported operand types: int + array
TypeError: Unsupported operand types: array + float
TypeError: Unsupported operand types: float + array
TypeError: Unsupported operand types: array + string
TypeError: Unsupported operand types: string + array
TypeError: Unsupported operand types: array + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + array
TypeError: Unsupported operand types: stdClass + null
TypeError: Unsupported operand types: null + stdClass
TypeError: Unsupported operand types: stdClass + bool
TypeError: Unsupported operand types: bool + stdClass
TypeError: Unsupported operand types: stdClass + bool
TypeError: Unsupported operand types: bool + stdClass
TypeError: Unsupported operand types: stdClass + int
TypeError: Unsupported operand types: int + stdClass
TypeError: Unsupported operand types: stdClass + float
TypeError: Unsupported operand types: float + stdClass
TypeError: Unsupported operand types: stdClass + string
TypeError: Unsupported operand types: string + stdClass
TypeError: Unsupported operand types: stdClass + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + stdClass
TypeError: Unsupported operand types: resource + null
TypeError: Unsupported operand types: null + resource
TypeError: Unsupported operand types: resource + bool
TypeError: Unsupported operand types: bool + resource
TypeError: Unsupported operand types: resource + bool
TypeError: Unsupported operand types: bool + resource
TypeError: Unsupported operand types: resource + int
TypeError: Unsupported operand types: int + resource
TypeError: Unsupported operand types: resource + float
TypeError: Unsupported operand types: float + resource
TypeError: Unsupported operand types: resource + string
TypeError: Unsupported operand types: string + resource
TypeError: Unsupported operand types: resource + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + resource
TypeError: Unsupported operand types: string + null
TypeError: Unsupported operand types: null + string
TypeError: Unsupported operand types: string + bool
TypeError: Unsupported operand types: bool + string
TypeError: Unsupported operand types: string + bool
TypeError: Unsupported operand types: bool + string
TypeError: Unsupported operand types: string + int
TypeError: Unsupported operand types: int + string
TypeError: Unsupported operand types: string + float
TypeError: Unsupported operand types: float + string
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: string + string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string + string
TypeError: Unsupported operand types: array - array
TypeError: Unsupported operand types: array - stdClass
TypeError: Unsupported operand types: array - resource
TypeError: Unsupported operand types: array - string
TypeError: Unsupported operand types: stdClass - array
TypeError: Unsupported operand types: stdClass - stdClass
TypeError: Unsupported operand types: stdClass - resource
TypeError: Unsupported operand types: stdClass - string
TypeError: Unsupported operand types: resource - array
TypeError: Unsupported operand types: resource - stdClass
TypeError: Unsupported operand types: resource - resource
TypeError: Unsupported operand types: resource - string
TypeError: Unsupported operand types: string - array
TypeError: Unsupported operand types: string - stdClass
TypeError: Unsupported operand types: string - resource
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: array - null
TypeError: Unsupported operand types: null - array
TypeError: Unsupported operand types: array - bool
TypeError: Unsupported operand types: bool - array
TypeError: Unsupported operand types: array - bool
TypeError: Unsupported operand types: bool - array
TypeError: Unsupported operand types: array - int
TypeError: Unsupported operand types: int - array
TypeError: Unsupported operand types: array - float
TypeError: Unsupported operand types: float - array
TypeError: Unsupported operand types: array - string
TypeError: Unsupported operand types: string - array
TypeError: Unsupported operand types: array - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - array
TypeError: Unsupported operand types: stdClass - null
TypeError: Unsupported operand types: null - stdClass
TypeError: Unsupported operand types: stdClass - bool
TypeError: Unsupported operand types: bool - stdClass
TypeError: Unsupported operand types: stdClass - bool
TypeError: Unsupported operand types: bool - stdClass
TypeError: Unsupported operand types: stdClass - int
TypeError: Unsupported operand types: int - stdClass
TypeError: Unsupported operand types: stdClass - float
TypeError: Unsupported operand types: float - stdClass
TypeError: Unsupported operand types: stdClass - string
TypeError: Unsupported operand types: string - stdClass
TypeError: Unsupported operand types: stdClass - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - stdClass
TypeError: Unsupported operand types: resource - null
TypeError: Unsupported operand types: null - resource
TypeError: Unsupported operand types: resource - bool
TypeError: Unsupported operand types: bool - resource
TypeError: Unsupported operand types: resource - bool
TypeError: Unsupported operand types: bool - resource
TypeError: Unsupported operand types: resource - int
TypeError: Unsupported operand types: int - resource
TypeError: Unsupported operand types: resource - float
TypeError: Unsupported operand types: float - resource
TypeError: Unsupported operand types: resource - string
TypeError: Unsupported operand types: string - resource
TypeError: Unsupported operand types: resource - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - resource
TypeError: Unsupported operand types: string - null
TypeError: Unsupported operand types: null - string
TypeError: Unsupported operand types: string - bool
TypeError: Unsupported operand types: bool - string
TypeError: Unsupported operand types: string - bool
TypeError: Unsupported operand types: bool - string
TypeError: Unsupported operand types: string - int
TypeError: Unsupported operand types: int - string
TypeError: Unsupported operand types: string - float
TypeError: Unsupported operand types: float - string
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: string - string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string - string
TypeError: Unsupported operand types: array * array
TypeError: Unsupported operand types: array * stdClass
TypeError: Unsupported operand types: array * resource
TypeError: Unsupported operand types: array * string
TypeError: Unsupported operand types: stdClass * array
TypeError: Unsupported operand types: stdClass * stdClass
TypeError: Unsupported operand types: stdClass * resource
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: resource * array
TypeError: Unsupported operand types: resource * stdClass
TypeError: Unsupported operand types: resource * resource
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: string * array
TypeError: Unsupported operand types: string * stdClass
TypeError: Unsupported operand types: string * resource
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: array * null
TypeError: Unsupported operand types: null * array
TypeError: Unsupported operand types: array * bool
TypeError: Unsupported operand types: bool * array
TypeError: Unsupported operand types: array * bool
TypeError: Unsupported operand types: bool * array
TypeError: Unsupported operand types: array * int
TypeError: Unsupported operand types: int * array
TypeError: Unsupported operand types: array * float
TypeError: Unsupported operand types: float * array
TypeError: Unsupported operand types: array * string
TypeError: Unsupported operand types: string * array
TypeError: Unsupported operand types: array * string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string * array
TypeError: Unsupported operand types: stdClass * null
TypeError: Unsupported operand types: null * stdClass
TypeError: Unsupported operand types: stdClass * bool
TypeError: Unsupported operand types: bool * stdClass
TypeError: Unsupported operand types: stdClass * bool
TypeError: Unsupported operand types: bool * stdClass
TypeError: Unsupported operand types: stdClass * int
TypeError: Unsupported operand types: int * stdClass
TypeError: Unsupported operand types: stdClass * float
TypeError: Unsupported operand types: float * stdClass
TypeError: Unsupported operand types: stdClass * string
TypeError: Unsupported operand types: string * stdClass
TypeError: Unsupported operand types: stdClass * string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string * stdClass
TypeError: Unsupported operand types: resource * null
TypeError: Unsupported operand types: null * resource
TypeError: Unsupported operand types: resource * bool
TypeError: Unsupported operand types: bool * resource
TypeError: Unsupported operand types: resource * bool
TypeError: Unsupported operand types: bool * resource
TypeError: Unsupported operand types: resource * int
TypeError: Unsupported operand types: int * resource
TypeError: Unsupported operand types: resource * float
TypeError: Unsupported operand types: float * resource
TypeError: Unsupported operand types: resource * string
TypeError: Unsupported operand types: string * resource
TypeError: Unsupported operand types: resource * string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string * resource
TypeError: Unsupported operand types: string * null
TypeError: Unsupported operand types: null * string
TypeError: Unsupported operand types: string * bool
TypeError: Unsupported operand types: bool * string
TypeError: Unsupported operand types: string * bool
TypeError: Unsupported operand types: bool * string
TypeError: Unsupported operand types: string * int
TypeError: Unsupported operand types: int * string
TypeError: Unsupported operand types: string * float
TypeError: Unsupported operand types: float * string
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: string * string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string * string
TypeError: Unsupported operand types: array / array
TypeError: Unsupported operand types: array / stdClass
TypeError: Unsupported operand types: array / resource
TypeError: Unsupported operand types: array / string
TypeError: Unsupported operand types: stdClass / array
TypeError: Unsupported operand types: stdClass / stdClass
TypeError: Unsupported operand types: stdClass / resource
TypeError: Unsupported operand types: stdClass / string
TypeError: Unsupported operand types: resource / array
TypeError: Unsupported operand types: resource / stdClass
TypeError: Unsupported operand types: resource / resource
TypeError: Unsupported operand types: resource / string
TypeError: Unsupported operand types: string / array
TypeError: Unsupported operand types: string / stdClass
TypeError: Unsupported operand types: string / resource
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: array / null
TypeError: Unsupported operand types: null / array
TypeError: Unsupported operand types: array / bool
TypeError: Unsupported operand types: bool / array
TypeError: Unsupported operand types: array / bool
TypeError: Unsupported operand types: bool / array
TypeError: Unsupported operand types: array / int
TypeError: Unsupported operand types: int / array
TypeError: Unsupported operand types: array / float
TypeError: Unsupported operand types: float / array
TypeError: Unsupported operand types: array / string
TypeError: Unsupported operand types: string / array
TypeError: Unsupported operand types: array / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / array
TypeError: Unsupported operand types: stdClass / null
TypeError: Unsupported operand types: null / stdClass
TypeError: Unsupported operand types: stdClass / bool
TypeError: Unsupported operand types: bool / stdClass
TypeError: Unsupported operand types: stdClass / bool
TypeError: Unsupported operand types: bool / stdClass
TypeError: Unsupported operand types: stdClass / int
TypeError: Unsupported operand types: int / stdClass
TypeError: Unsupported operand types: stdClass / float
TypeError: Unsupported operand types: float / stdClass
TypeError: Unsupported operand types: stdClass / string
TypeError: Unsupported operand types: string / stdClass
TypeError: Unsupported operand types: stdClass / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / stdClass
TypeError: Unsupported operand types: resource / null
TypeError: Unsupported operand types: null / resource
TypeError: Unsupported operand types: resource / bool
TypeError: Unsupported operand types: bool / resource
TypeError: Unsupported operand types: resource / bool
TypeError: Unsupported operand types: bool / resource
TypeError: Unsupported operand types: resource / int
TypeError: Unsupported operand types: int / resource
TypeError: Unsupported operand types: resource / float
TypeError: Unsupported operand types: float / resource
TypeError: Unsupported operand types: resource / string
TypeError: Unsupported operand types: string / resource
TypeError: Unsupported operand types: resource / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / resource
TypeError: Unsupported operand types: string / null
TypeError: Unsupported operand types: null / string
TypeError: Unsupported operand types: string / bool
TypeError: Unsupported operand types: bool / string
TypeError: Unsupported operand types: string / bool
TypeError: Unsupported operand types: bool / string
TypeError: Unsupported operand types: string / int
TypeError: Unsupported operand types: int / string
TypeError: Unsupported operand types: string / float
TypeError: Unsupported operand types: float / string
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: string / string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string / string
TypeError: Unsupported operand types: array % array
TypeError: Unsupported operand types: array % stdClass
TypeError: Unsupported operand types: array % resource
TypeError: Unsupported operand types: array % string
TypeError: Unsupported operand types: stdClass % array
TypeError: Unsupported operand types: stdClass % stdClass
TypeError: Unsupported operand types: stdClass % resource
TypeError: Unsupported operand types: stdClass % string
TypeError: Unsupported operand types: resource % array
TypeError: Unsupported operand types: resource % stdClass
TypeError: Unsupported operand types: resource % resource
TypeError: Unsupported operand types: resource % string
TypeError: Unsupported operand types: string % array
TypeError: Unsupported operand types: string % stdClass
TypeError: Unsupported operand types: string % resource
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: array % null
TypeError: Unsupported operand types: null % array
TypeError: Unsupported operand types: array % bool
TypeError: Unsupported operand types: bool % array
TypeError: Unsupported operand types: array % bool
TypeError: Unsupported operand types: bool % array
TypeError: Unsupported operand types: array % int
TypeError: Unsupported operand types: int % array
TypeError: Unsupported operand types: array % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % array
TypeError: Unsupported operand types: array % string
TypeError: Unsupported operand types: string % array
TypeError: Unsupported operand types: array % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % array
TypeError: Unsupported operand types: stdClass % null
TypeError: Unsupported operand types: null % stdClass
TypeError: Unsupported operand types: stdClass % bool
TypeError: Unsupported operand types: bool % stdClass
TypeError: Unsupported operand types: stdClass % bool
TypeError: Unsupported operand types: bool % stdClass
TypeError: Unsupported operand types: stdClass % int
TypeError: Unsupported operand types: int % stdClass
TypeError: Unsupported operand types: stdClass % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % stdClass
TypeError: Unsupported operand types: stdClass % string
TypeError: Unsupported operand types: string % stdClass
TypeError: Unsupported operand types: stdClass % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % stdClass
TypeError: Unsupported operand types: resource % null
TypeError: Unsupported operand types: null % resource
TypeError: Unsupported operand types: resource % bool
TypeError: Unsupported operand types: bool % resource
TypeError: Unsupported operand types: resource % bool
TypeError: Unsupported operand types: bool % resource
TypeError: Unsupported operand types: resource % int
TypeError: Unsupported operand types: int % resource
TypeError: Unsupported operand types: resource % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % resource
TypeError: Unsupported operand types: resource % string
TypeError: Unsupported operand types: string % resource
TypeError: Unsupported operand types: resource % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % resource
TypeError: Unsupported operand types: string % null
TypeError: Unsupported operand types: null % string
TypeError: Unsupported operand types: string % bool
TypeError: Unsupported operand types: bool % string
TypeError: Unsupported operand types: string % bool
TypeError: Unsupported operand types: bool % string
TypeError: Unsupported operand types: string % int
TypeError: Unsupported operand types: int % string
TypeError: Unsupported operand types: string % float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float % string
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: string % string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string % string
TypeError: Unsupported operand types: array ** array
TypeError: Unsupported operand types: array ** stdClass
TypeError: Unsupported operand types: array ** resource
TypeError: Unsupported operand types: array ** string
TypeError: Unsupported operand types: stdClass ** array
TypeError: Unsupported operand types: stdClass ** stdClass
TypeError: Unsupported operand types: stdClass ** resource
TypeError: Unsupported operand types: stdClass ** string
TypeError: Unsupported operand types: resource ** array
TypeError: Unsupported operand types: resource ** stdClass
TypeError: Unsupported operand types: resource ** resource
TypeError: Unsupported operand types: resource ** string
TypeError: Unsupported operand types: string ** array
TypeError: Unsupported operand types: string ** stdClass
TypeError: Unsupported operand types: string ** resource
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: array ** null
TypeError: Unsupported operand types: null ** array
TypeError: Unsupported operand types: array ** bool
TypeError: Unsupported operand types: bool ** array
TypeError: Unsupported operand types: array ** bool
TypeError: Unsupported operand types: bool ** array
TypeError: Unsupported operand types: array ** int
TypeError: Unsupported operand types: int ** array
TypeError: Unsupported operand types: array ** float
TypeError: Unsupported operand types: float ** array
TypeError: Unsupported operand types: array ** string
TypeError: Unsupported operand types: string ** array
TypeError: Unsupported operand types: array ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** array
TypeError: Unsupported operand types: stdClass ** null
TypeError: Unsupported operand types: null ** stdClass
TypeError: Unsupported operand types: stdClass ** bool
TypeError: Unsupported operand types: bool ** stdClass
TypeError: Unsupported operand types: stdClass ** bool
TypeError: Unsupported operand types: bool ** stdClass
TypeError: Unsupported operand types: stdClass ** int
TypeError: Unsupported operand types: int ** stdClass
TypeError: Unsupported operand types: stdClass ** float
TypeError: Unsupported operand types: float ** stdClass
TypeError: Unsupported operand types: stdClass ** string
TypeError: Unsupported operand types: string ** stdClass
TypeError: Unsupported operand types: stdClass ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** stdClass
TypeError: Unsupported operand types: resource ** null
TypeError: Unsupported operand types: null ** resource
TypeError: Unsupported operand types: resource ** bool
TypeError: Unsupported operand types: bool ** resource
TypeError: Unsupported operand types: resource ** bool
TypeError: Unsupported operand types: bool ** resource
TypeError: Unsupported operand types: resource ** int
TypeError: Unsupported operand types: int ** resource
TypeError: Unsupported operand types: resource ** float
TypeError: Unsupported operand types: float ** resource
TypeError: Unsupported operand types: resource ** string
TypeError: Unsupported operand types: string ** resource
TypeError: Unsupported operand types: resource ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** resource
TypeError: Unsupported operand types: string ** null
TypeError: Unsupported operand types: null ** string
TypeError: Unsupported operand types: string ** bool
TypeError: Unsupported operand types: bool ** string
TypeError: Unsupported operand types: string ** bool
TypeError: Unsupported operand types: bool ** string
TypeError: Unsupported operand types: string ** int
TypeError: Unsupported operand types: int ** string
TypeError: Unsupported operand types: string ** float
TypeError: Unsupported operand types: float ** string
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: string ** string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ** string
TypeError: Unsupported operand types: array << array
TypeError: Unsupported operand types: array << stdClass
TypeError: Unsupported operand types: array << resource
TypeError: Unsupported operand types: array << string
TypeError: Unsupported operand types: stdClass << array
TypeError: Unsupported operand types: stdClass << stdClass
TypeError: Unsupported operand types: stdClass << resource
TypeError: Unsupported operand types: stdClass << string
TypeError: Unsupported operand types: resource << array
TypeError: Unsupported operand types: resource << stdClass
TypeError: Unsupported operand types: resource << resource
TypeError: Unsupported operand types: resource << string
TypeError: Unsupported operand types: string << array
TypeError: Unsupported operand types: string << stdClass
TypeError: Unsupported operand types: string << resource
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: array << null
TypeError: Unsupported operand types: null << array
TypeError: Unsupported operand types: array << bool
TypeError: Unsupported operand types: bool << array
TypeError: Unsupported operand types: array << bool
TypeError: Unsupported operand types: bool << array
TypeError: Unsupported operand types: array << int
TypeError: Unsupported operand types: int << array
TypeError: Unsupported operand types: array << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << array
TypeError: Unsupported operand types: array << string
TypeError: Unsupported operand types: string << array
TypeError: Unsupported operand types: array << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << array
TypeError: Unsupported operand types: stdClass << null
TypeError: Unsupported operand types: null << stdClass
TypeError: Unsupported operand types: stdClass << bool
TypeError: Unsupported operand types: bool << stdClass
TypeError: Unsupported operand types: stdClass << bool
TypeError: Unsupported operand types: bool << stdClass
TypeError: Unsupported operand types: stdClass << int
TypeError: Unsupported operand types: int << stdClass
TypeError: Unsupported operand types: stdClass << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << stdClass
TypeError: Unsupported operand types: stdClass << string
TypeError: Unsupported operand types: string << stdClass
TypeError: Unsupported operand types: stdClass << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << stdClass
TypeError: Unsupported operand types: resource << null
TypeError: Unsupported operand types: null << resource
TypeError: Unsupported operand types: resource << bool
TypeError: Unsupported operand types: bool << resource
TypeError: Unsupported operand types: resource << bool
TypeError: Unsupported operand types: bool << resource
TypeError: Unsupported operand types: resource << int
TypeError: Unsupported operand types: int << resource
TypeError: Unsupported operand types: resource << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << resource
TypeError: Unsupported operand types: resource << string
TypeError: Unsupported operand types: string << resource
TypeError: Unsupported operand types: resource << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << resource
TypeError: Unsupported operand types: string << null
TypeError: Unsupported operand types: null << string
TypeError: Unsupported operand types: string << bool
TypeError: Unsupported operand types: bool << string
TypeError: Unsupported operand types: string << bool
TypeError: Unsupported operand types: bool << string
TypeError: Unsupported operand types: string << int
TypeError: Unsupported operand types: int << string
TypeError: Unsupported operand types: string << float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float << string
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: string << string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string << string
TypeError: Unsupported operand types: array >> array
TypeError: Unsupported operand types: array >> stdClass
TypeError: Unsupported operand types: array >> resource
TypeError: Unsupported operand types: array >> string
TypeError: Unsupported operand types: stdClass >> array
TypeError: Unsupported operand types: stdClass >> stdClass
TypeError: Unsupported operand types: stdClass >> resource
TypeError: Unsupported operand types: stdClass >> string
TypeError: Unsupported operand types: resource >> array
TypeError: Unsupported operand types: resource >> stdClass
TypeError: Unsupported operand types: resource >> resource
TypeError: Unsupported operand types: resource >> string
TypeError: Unsupported operand types: string >> array
TypeError: Unsupported operand types: string >> stdClass
TypeError: Unsupported operand types: string >> resource
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: array >> null
TypeError: Unsupported operand types: null >> array
TypeError: Unsupported operand types: array >> bool
TypeError: Unsupported operand types: bool >> array
TypeError: Unsupported operand types: array >> bool
TypeError: Unsupported operand types: bool >> array
TypeError: Unsupported operand types: array >> int
TypeError: Unsupported operand types: int >> array
TypeError: Unsupported operand types: array >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> array
TypeError: Unsupported operand types: array >> string
TypeError: Unsupported operand types: string >> array
TypeError: Unsupported operand types: array >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> array
TypeError: Unsupported operand types: stdClass >> null
TypeError: Unsupported operand types: null >> stdClass
TypeError: Unsupported operand types: stdClass >> bool
TypeError: Unsupported operand types: bool >> stdClass
TypeError: Unsupported operand types: stdClass >> bool
TypeError: Unsupported operand types: bool >> stdClass
TypeError: Unsupported operand types: stdClass >> int
TypeError: Unsupported operand types: int >> stdClass
TypeError: Unsupported operand types: stdClass >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> stdClass
TypeError: Unsupported operand types: stdClass >> string
TypeError: Unsupported operand types: string >> stdClass
TypeError: Unsupported operand types: stdClass >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> stdClass
TypeError: Unsupported operand types: resource >> null
TypeError: Unsupported operand types: null >> resource
TypeError: Unsupported operand types: resource >> bool
TypeError: Unsupported operand types: bool >> resource
TypeError: Unsupported operand types: resource >> bool
TypeError: Unsupported operand types: bool >> resource
TypeError: Unsupported operand types: resource >> int
TypeError: Unsupported operand types: int >> resource
TypeError: Unsupported operand types: resource >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> resource
TypeError: Unsupported operand types: resource >> string
TypeError: Unsupported operand types: string >> resource
TypeError: Unsupported operand types: resource >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> resource
TypeError: Unsupported operand types: string >> null
TypeError: Unsupported operand types: null >> string
TypeError: Unsupported operand types: string >> bool
TypeError: Unsupported operand types: bool >> string
TypeError: Unsupported operand types: string >> bool
TypeError: Unsupported operand types: bool >> string
TypeError: Unsupported operand types: string >> int
TypeError: Unsupported operand types: int >> string
TypeError: Unsupported operand types: string >> float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float >> string
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: string >> string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string >> string
TypeError: Unsupported operand types: array & array
TypeError: Unsupported operand types: array & stdClass
TypeError: Unsupported operand types: array & resource
TypeError: Unsupported operand types: array & string
TypeError: Unsupported operand types: stdClass & array
TypeError: Unsupported operand types: stdClass & stdClass
TypeError: Unsupported operand types: stdClass & resource
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: resource & array
TypeError: Unsupported operand types: resource & stdClass
TypeError: Unsupported operand types: resource & resource
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: string & array
TypeError: Unsupported operand types: string & stdClass
TypeError: Unsupported operand types: string & resource
No error for "foo" &= "foo"
TypeError: Unsupported operand types: array & null
TypeError: Unsupported operand types: null & array
TypeError: Unsupported operand types: array & bool
TypeError: Unsupported operand types: bool & array
TypeError: Unsupported operand types: array & bool
TypeError: Unsupported operand types: bool & array
TypeError: Unsupported operand types: array & int
TypeError: Unsupported operand types: int & array
TypeError: Unsupported operand types: array & float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float & array
TypeError: Unsupported operand types: array & string
TypeError: Unsupported operand types: string & array
TypeError: Unsupported operand types: array & string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string & array
TypeError: Unsupported operand types: stdClass & null
TypeError: Unsupported operand types: null & stdClass
TypeError: Unsupported operand types: stdClass & bool
TypeError: Unsupported operand types: bool & stdClass
TypeError: Unsupported operand types: stdClass & bool
TypeError: Unsupported operand types: bool & stdClass
TypeError: Unsupported operand types: stdClass & int
TypeError: Unsupported operand types: int & stdClass
TypeError: Unsupported operand types: stdClass & float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float & stdClass
TypeError: Unsupported operand types: stdClass & string
TypeError: Unsupported operand types: string & stdClass
TypeError: Unsupported operand types: stdClass & string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string & stdClass
TypeError: Unsupported operand types: resource & null
TypeError: Unsupported operand types: null & resource
TypeError: Unsupported operand types: resource & bool
TypeError: Unsupported operand types: bool & resource
TypeError: Unsupported operand types: resource & bool
TypeError: Unsupported operand types: bool & resource
TypeError: Unsupported operand types: resource & int
TypeError: Unsupported operand types: int & resource
TypeError: Unsupported operand types: resource & float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float & resource
TypeError: Unsupported operand types: resource & string
TypeError: Unsupported operand types: string & resource
TypeError: Unsupported operand types: resource & string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string & resource
TypeError: Unsupported operand types: string & null
TypeError: Unsupported operand types: null & string
TypeError: Unsupported operand types: string & bool
TypeError: Unsupported operand types: bool & string
TypeError: Unsupported operand types: string & bool
TypeError: Unsupported operand types: bool & string
TypeError: Unsupported operand types: string & int
TypeError: Unsupported operand types: int & string
TypeError: Unsupported operand types: string & float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float & string
No error for "foo" &= "123"
No error for "123" &= "foo"
No error for "foo" &= "123foo"
No error for "123foo" &= "foo"
TypeError: Unsupported operand types: array | array
TypeError: Unsupported operand types: array | stdClass
TypeError: Unsupported operand types: array | resource
TypeError: Unsupported operand types: array | string
TypeError: Unsupported operand types: stdClass | array
TypeError: Unsupported operand types: stdClass | stdClass
TypeError: Unsupported operand types: stdClass | resource
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: resource | array
TypeError: Unsupported operand types: resource | stdClass
TypeError: Unsupported operand types: resource | resource
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: string | array
TypeError: Unsupported operand types: string | stdClass
TypeError: Unsupported operand types: string | resource
No error for "foo" |= "foo"
TypeError: Unsupported operand types: array | null
TypeError: Unsupported operand types: null | array
TypeError: Unsupported operand types: array | bool
TypeError: Unsupported operand types: bool | array
TypeError: Unsupported operand types: array | bool
TypeError: Unsupported operand types: bool | array
TypeError: Unsupported operand types: array | int
TypeError: Unsupported operand types: int | array
TypeError: Unsupported operand types: array | float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float | array
TypeError: Unsupported operand types: array | string
TypeError: Unsupported operand types: string | array
TypeError: Unsupported operand types: array | string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string | array
TypeError: Unsupported operand types: stdClass | null
TypeError: Unsupported operand types: null | stdClass
TypeError: Unsupported operand types: stdClass | bool
TypeError: Unsupported operand types: bool | stdClass
TypeError: Unsupported operand types: stdClass | bool
TypeError: Unsupported operand types: bool | stdClass
TypeError: Unsupported operand types: stdClass | int
TypeError: Unsupported operand types: int | stdClass
TypeError: Unsupported operand types: stdClass | float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float | stdClass
TypeError: Unsupported operand types: stdClass | string
TypeError: Unsupported operand types: string | stdClass
TypeError: Unsupported operand types: stdClass | string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string | stdClass
TypeError: Unsupported operand types: resource | null
TypeError: Unsupported operand types: null | resource
TypeError: Unsupported operand types: resource | bool
TypeError: Unsupported operand types: bool | resource
TypeError: Unsupported operand types: resource | bool
TypeError: Unsupported operand types: bool | resource
TypeError: Unsupported operand types: resource | int
TypeError: Unsupported operand types: int | resource
TypeError: Unsupported operand types: resource | float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float | resource
TypeError: Unsupported operand types: resource | string
TypeError: Unsupported operand types: string | resource
TypeError: Unsupported operand types: resource | string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string | resource
TypeError: Unsupported operand types: string | null
TypeError: Unsupported operand types: null | string
TypeError: Unsupported operand types: string | bool
TypeError: Unsupported operand types: bool | string
TypeError: Unsupported operand types: string | bool
TypeError: Unsupported operand types: bool | string
TypeError: Unsupported operand types: string | int
TypeError: Unsupported operand types: int | string
TypeError: Unsupported operand types: string | float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float | string
No error for "foo" |= "123"
No error for "123" |= "foo"
No error for "foo" |= "123foo"
No error for "123foo" |= "foo"
TypeError: Unsupported operand types: array ^ array
TypeError: Unsupported operand types: array ^ stdClass
TypeError: Unsupported operand types: array ^ resource
TypeError: Unsupported operand types: array ^ string
TypeError: Unsupported operand types: stdClass ^ array
TypeError: Unsupported operand types: stdClass ^ stdClass
TypeError: Unsupported operand types: stdClass ^ resource
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: resource ^ array
TypeError: Unsupported operand types: resource ^ stdClass
TypeError: Unsupported operand types: resource ^ resource
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: string ^ array
TypeError: Unsupported operand types: string ^ stdClass
TypeError: Unsupported operand types: string ^ resource
No error for "foo" ^= "foo"
TypeError: Unsupported operand types: array ^ null
TypeError: Unsupported operand types: null ^ array
TypeError: Unsupported operand types: array ^ bool
TypeError: Unsupported operand types: bool ^ array
TypeError: Unsupported operand types: array ^ bool
TypeError: Unsupported operand types: bool ^ array
TypeError: Unsupported operand types: array ^ int
TypeError: Unsupported operand types: int ^ array
TypeError: Unsupported operand types: array ^ float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float ^ array
TypeError: Unsupported operand types: array ^ string
TypeError: Unsupported operand types: string ^ array
TypeError: Unsupported operand types: array ^ string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ^ array
TypeError: Unsupported operand types: stdClass ^ null
TypeError: Unsupported operand types: null ^ stdClass
TypeError: Unsupported operand types: stdClass ^ bool
TypeError: Unsupported operand types: bool ^ stdClass
TypeError: Unsupported operand types: stdClass ^ bool
TypeError: Unsupported operand types: bool ^ stdClass
TypeError: Unsupported operand types: stdClass ^ int
TypeError: Unsupported operand types: int ^ stdClass
TypeError: Unsupported operand types: stdClass ^ float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float ^ stdClass
TypeError: Unsupported operand types: stdClass ^ string
TypeError: Unsupported operand types: string ^ stdClass
TypeError: Unsupported operand types: stdClass ^ string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ^ stdClass
TypeError: Unsupported operand types: resource ^ null
TypeError: Unsupported operand types: null ^ resource
TypeError: Unsupported operand types: resource ^ bool
TypeError: Unsupported operand types: bool ^ resource
TypeError: Unsupported operand types: resource ^ bool
TypeError: Unsupported operand types: bool ^ resource
TypeError: Unsupported operand types: resource ^ int
TypeError: Unsupported operand types: int ^ resource
TypeError: Unsupported operand types: resource ^ float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float ^ resource
TypeError: Unsupported operand types: resource ^ string
TypeError: Unsupported operand types: string ^ resource
TypeError: Unsupported operand types: resource ^ string
Warning: A non-numeric value encountered
TypeError: Unsupported operand types: string ^ resource
TypeError: Unsupported operand types: string ^ null
TypeError: Unsupported operand types: null ^ string
TypeError: Unsupported operand types: string ^ bool
TypeError: Unsupported operand types: bool ^ string
TypeError: Unsupported operand types: string ^ bool
TypeError: Unsupported operand types: bool ^ string
TypeError: Unsupported operand types: string ^ int
TypeError: Unsupported operand types: int ^ string
TypeError: Unsupported operand types: string ^ float
Warning: Implicit conversion from float 3.5 to int loses precision
TypeError: Unsupported operand types: float ^ string
No error for "foo" ^= "123"
No error for "123" ^= "foo"
No error for "foo" ^= "123foo"
No error for "123foo" ^= "foo"
Warning: Array to string conversion
Warning: Array to string conversion
No error for [] .= []
Warning: Array to string conversion
Error: Object of class stdClass could not be converted to string
Warning: Array to string conversion
No error for [] .= STDOUT
Warning: Array to string conversion
No error for [] .= "foo"
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Warning: Array to string conversion
No error for STDOUT .= []
Error: Object of class stdClass could not be converted to string
No error for STDOUT .= STDOUT
No error for STDOUT .= "foo"
Warning: Array to string conversion
No error for "foo" .= []
Error: Object of class stdClass could not be converted to string
No error for "foo" .= STDOUT
No error for "foo" .= "foo"
Warning: Array to string conversion
No error for [] .= null
Warning: Array to string conversion
No error for null .= []
Warning: Array to string conversion
No error for [] .= true
Warning: Array to string conversion
No error for true .= []
Warning: Array to string conversion
No error for [] .= false
Warning: Array to string conversion
No error for false .= []
Warning: Array to string conversion
No error for [] .= 2
Warning: Array to string conversion
No error for 2 .= []
Warning: Array to string conversion
No error for [] .= 3.5
Warning: Array to string conversion
No error for 3.5 .= []
Warning: Array to string conversion
No error for [] .= "123"
Warning: Array to string conversion
No error for "123" .= []
Warning: Array to string conversion
No error for [] .= "123foo"
Warning: Array to string conversion
No error for "123foo" .= []
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
Error: Object of class stdClass could not be converted to string
No error for STDOUT .= null
No error for null .= STDOUT
No error for STDOUT .= true
No error for true .= STDOUT
No error for STDOUT .= false
No error for false .= STDOUT
No error for STDOUT .= 2
No error for 2 .= STDOUT
No error for STDOUT .= 3.5
No error for 3.5 .= STDOUT
No error for STDOUT .= "123"
No error for "123" .= STDOUT
No error for STDOUT .= "123foo"
No error for "123foo" .= STDOUT
No error for "foo" .= null
No error for null .= "foo"
No error for "foo" .= true
No error for true .= "foo"
No error for "foo" .= false
No error for false .= "foo"
No error for "foo" .= 2
No error for 2 .= "foo"
No error for "foo" .= 3.5
No error for 3.5 .= "foo"
No error for "foo" .= "123"
No error for "123" .= "foo"
No error for "foo" .= "123foo"
No error for "123foo" .= "foo"


UNARY OP:
TypeError: Cannot perform bitwise not on array
TypeError: Cannot perform bitwise not on stdClass
TypeError: Cannot perform bitwise not on resource
No error for ~"foo"


INCDEC:
TypeError: Cannot increment array
TypeError: Cannot decrement array
TypeError: Cannot increment stdClass
TypeError: Cannot decrement stdClass
TypeError: Cannot increment resource
TypeError: Cannot decrement resource
Warning: Increment on non-numeric string is deprecated, use str_increment() instead
No error for fop++
Warning: Decrement on non-numeric string has no effect and is deprecated
No error for foo--
