--TEST--
pack() integer value conversions
--FILE--
<?php

$formats = ['c', 'C', 's', 'S', 'n', 'v', 'i', 'I', 'l', 'L', 'N', 'V'];
if (PHP_INT_SIZE >= 8) {
    array_push($formats, 'q', 'Q', 'J', 'P');
}

$values = [
    'null' => [null, 0],
    'false' => [false, 0],
    'true' => [true, 1],
    'int' => [42, 42],
    'integral float' => [42.0, 42],
    'numeric integer string' => ['42', 42],
    'numeric float string' => ['42.0', 42],
    'numeric scientific string' => ['4.2e1', 42],
    'leading whitespace' => [' 42', 42],
    'trailing whitespace' => ["42 \t", 42],
];

$passed = true;
foreach ($formats as $format) {
    foreach ($values as $name => [$value, $expected]) {
        $actual = unpack($format, pack($format, $value))[1];
        if ($actual !== $expected) {
            echo "Unexpected result for $format/$name: ";
            var_dump($actual);
            $passed = false;
        }
    }
}
echo "valid conversions: ";
var_dump($passed);

$passed = true;
$value = 42;
$reference =& $value;
foreach ($formats as $format) {
    $actual = unpack($format, pack($format, $reference))[1];
    if ($actual !== 42) {
        echo "Unexpected result for $format/reference: ";
        var_dump($actual);
        $passed = false;
    }
}
echo "references: ";
var_dump($passed);

$precisionLossValues = [
    'float' => [42.5, 'Implicit conversion from float 42.5 to int loses precision'],
    'float string' => ['42.5', 'Implicit conversion from float-string "42.5" to int loses precision'],
];

$passed = true;
foreach ($formats as $format) {
    foreach ($precisionLossValues as $name => [$value, $message]) {
        $diagnostic = null;
        set_error_handler(static function (int $errno, string $errstr) use (&$diagnostic): bool {
            $diagnostic = [$errno, $errstr];
            return true;
        });
        $actual = unpack($format, pack($format, $value))[1];
        restore_error_handler();

        if ($actual !== 42 || $diagnostic !== [E_DEPRECATED, $message]) {
            echo "Unexpected precision-loss result for $format/$name: ";
            var_dump($actual, $diagnostic);
            $passed = false;
        }
    }
}
echo "precision loss: ";
var_dump($passed);

$passed = true;
foreach ($formats as $format) {
    $diagnostic = null;
    set_error_handler(static function (int $errno, string $errstr) use (&$diagnostic): bool {
        $diagnostic = [$errno, $errstr];
        return true;
    });
    $actual = unpack($format, pack($format, '42 with trailing data'))[1];
    restore_error_handler();

    if ($actual !== 42 || $diagnostic !== [E_WARNING, 'A non-numeric value encountered']) {
        echo "Unexpected trailing-data result for $format: ";
        var_dump($actual, $diagnostic);
        $passed = false;
    }
}
echo "trailing data: ";
var_dump($passed);

?>
--EXPECT--
valid conversions: bool(true)
references: bool(true)
precision loss: bool(true)
trailing data: bool(true)
