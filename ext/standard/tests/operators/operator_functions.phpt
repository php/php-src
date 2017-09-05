--TEST--
Operator functions
--FILE--
<?php

$unary_operators = [
    '+',
    '-',
    '~',
    '!'
];

$binary_operators = [
    '+',
    '-',
    '*',
    '/',
    '%',
    '**',
    '&',
    '|',
    '^',
    '<<',
    '>>',
    '==',
    '===',
    '!=',
    '<>', // alias of '!='
    '!==',
    '<',
    '>',
    '<=',
    '>=',
    '<=>',
    'and',
    '&&',
    'or',
    '||',
    'xor',
    '.'
];

$values = [
    -1,
    0,
    1,
    3,
    0.0,
    -0.0,
    INF,
    NAN,
    "123",
    "123abc",
    "  123",
    "abc123",
    TRUE,
    FALSE,
    NULL
];

$errors = "";

set_error_handler(function (int $errno, string $errstr) use (&$errors) {
    $errors .= "$errno: $errstr" . PHP_EOL;
});

function test_unary(string $op, $value): void {
    global $unary;
    $valueSource = var_export($value, TRUE);
    $source1 = "return $op($valueSource);";
    $source2 = "return '$op'($valueSource);";
    $errors = '';
    $result1 = var_export(eval($source1), TRUE);
    $errors1 = $errors;
    $errors = '';
    $result2 = var_export(eval($source2), TRUE);
    $errors2 = $errors;
    if ($result1 !== $result2) {
        echo "ERROR: Return value mismatch: $source1 ($result1) !== $source2 ($result2)", PHP_EOL;
    }
    if ($errors1 !== $errors2) {
        echo "ERROR: Error mismatch: $source1 ($errors1) !== $source2 ($errors2)", PHP_EOL;
    }
}

function test_binary(string $op, $value1, $value2): void {
    global $errors;
    $valueSource1 = var_export($value1, TRUE);
    $valueSource2 = var_export($value2, TRUE);
    $source1 = "return ($valueSource1) $op ($valueSource2);";
    $source2 = "return '$op'($valueSource1, $valueSource2);";
    $errors = '';
    try {
        $result1 = var_export(eval($source1), TRUE);
    } catch (Throwable $e) {
        $errors .= get_class($e) . ': ' . $e->getMessage() . PHP_EOL;
    }
    $errors1 = $errors;
    $errors = '';
    try {
        $result2 = var_export(eval($source2), TRUE);
    } catch (Throwable $e) {
        $errors .= get_class($e) . ': ' . $e->getMessage() . PHP_EOL;
    }
    $errors2 = $errors;
    if ($result1 !== $result2) {
        echo "ERROR: Return value mismatch: $source1 ($result1) !== $source2 ($result2)", PHP_EOL;
    }
    if ($errors1 !== $errors2) {
        echo "ERROR: Error mismatch: $source1 ($errors1) !== $source2 ($errors2)", PHP_EOL;
    }
}

foreach ($unary_operators as $op) {
    foreach ($values as $value) {
        // ~ is very picky about types
        if ($op === '~' && ($value === NULL || is_bool($value))) {
            continue;
        }
        test_unary($op, $value);
    }
}

foreach ($binary_operators as $op) {
    foreach ($values as $value1) {
        foreach ($values as $value2) {
            test_binary($op, $value1, $value2);
        }
    }
}

test_binary('+', ["a" => 1, "b" => 2], ["a" => 3, "b" => 4, "c" => 5]);
test_binary('+', ["a" => 3, "b" => 4, "c" => 5], ["a" => 1, "b" => 2]);

echo "DONE", PHP_EOL;

?>
--EXPECTF--
DONE
