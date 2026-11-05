--TEST--
pack() integer value conversion errors
--FILE--
<?php

$formats = ['c', 'C', 's', 'S', 'n', 'v', 'i', 'I', 'l', 'L', 'N', 'V'];
if (PHP_INT_SIZE >= 8) {
    array_push($formats, 'q', 'Q', 'J', 'P');
}

$passed = true;
foreach ($formats as $format) {
    try {
        pack($format, []);
        echo "Unexpectedly accepted an array for $format\n";
        $passed = false;
    } catch (Throwable $e) {
        $expected = "pack(): Argument #2 must be of type int for format code '$format', array given";
        if (!$e instanceof TypeError || $e->getMessage() !== $expected) {
            echo "Unexpected exception for $format: ", $e::class, ': ', $e->getMessage(), "\n";
            $passed = false;
        }
    }
}
echo "all formats: ";
var_dump($passed);

$resource = fopen(__FILE__, 'r');
$invalidValues = [
    'empty string' => ['', 'string'],
    'whitespace string' => [' ', 'string'],
    'non-numeric string' => ['not numeric', 'string'],
    'empty array' => [[], 'array'],
    'non-empty array' => [[1], 'array'],
    'object' => [new stdClass(), 'stdClass'],
    'resource' => [$resource, 'resource'],
];

$passed = true;
foreach ($invalidValues as $name => [$value, $type]) {
    try {
        pack('i', $value);
        echo "Unexpectedly accepted $name\n";
        $passed = false;
    } catch (Throwable $e) {
        $expected = "pack(): Argument #2 must be of type int for format code 'i', $type given";
        if (!$e instanceof TypeError || $e->getMessage() !== $expected) {
            echo "Unexpected exception for $name: ", $e::class, ': ', $e->getMessage(), "\n";
            $passed = false;
        }
    }
}
echo "invalid values: ";
var_dump($passed);

$passed = true;
$value = [];
$reference =& $value;
foreach ($formats as $format) {
    try {
        pack($format, $reference);
        echo "Unexpectedly accepted an invalid reference for $format\n";
        $passed = false;
    } catch (Throwable $e) {
        $expected = "pack(): Argument #2 must be of type int for format code '$format', array given";
        if (!$e instanceof TypeError || $e->getMessage() !== $expected) {
            echo "Unexpected exception for $format/reference: ", $e::class, ': ', $e->getMessage(), "\n";
            $passed = false;
        }
    }
}
echo "invalid references: ";
var_dump($passed);

$passed = true;
$cases = [
    ['i2', [1, []], 3, 'i'],
    ['i*', [1, []], 3, 'i'],
    ['C2i', [1, 2, []], 4, 'i'],
    ['i2l2', [1, 2, 3, []], 5, 'l'],
];
foreach ($cases as [$format, $arguments, $argumentNumber, $valueFormat]) {
    try {
        pack($format, ...$arguments);
        echo "Unexpectedly accepted an invalid argument for $format\n";
        $passed = false;
    } catch (Throwable $e) {
        $expected = "pack(): Argument #$argumentNumber must be of type int for format code '$valueFormat', array given";
        if (!$e instanceof TypeError || $e->getMessage() !== $expected) {
            echo "Unexpected exception for $format: ", $e::class, ': ', $e->getMessage(), "\n";
            $passed = false;
        }
    }
}
echo "argument numbers: ";
var_dump($passed);

$passed = true;
$diagnosticValues = [
    'float' => [42.5, E_DEPRECATED, 'Implicit conversion from float 42.5 to int loses precision'],
    'float string' => ['42.5', E_DEPRECATED, 'Implicit conversion from float-string "42.5" to int loses precision'],
    'leading-numeric string' => ['42 with trailing data', E_WARNING, 'A non-numeric value encountered'],
];
set_error_handler(static function (int $errno, string $errstr): never {
    throw new Exception($errstr, $errno);
});
foreach ($formats as $format) {
    foreach ($diagnosticValues as $name => [$value, $severity, $message]) {
        try {
            pack($format, $value);
            echo "Unexpectedly accepted $name for $format\n";
            $passed = false;
        } catch (Throwable $e) {
            if (!$e instanceof Exception || $e->getCode() !== $severity || $e->getMessage() !== $message) {
                echo "Unexpected exception for $format/$name: ", $e::class, ': ', $e->getMessage(), "\n";
                $passed = false;
            }
        }
    }
}
restore_error_handler();
fclose($resource);
echo "exception propagation: ";
var_dump($passed);

?>
--EXPECT--
all formats: bool(true)
invalid values: bool(true)
invalid references: bool(true)
argument numbers: bool(true)
exception propagation: bool(true)
