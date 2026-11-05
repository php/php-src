--TEST--
pack() accepts GMP values for numeric format codes
--EXTENSIONS--
gmp
--FILE--
<?php

$integerFormats = ['c', 'C', 's', 'S', 'n', 'v', 'i', 'I', 'l', 'L', 'N', 'V'];
if (PHP_INT_SIZE >= 8) {
    array_push($integerFormats, 'q', 'Q', 'J', 'P');
}
$floatFormats = ['f', 'g', 'G', 'd', 'e', 'E'];
$value = gmp_init(42);

$passed = true;
foreach ($integerFormats as $format) {
    $actual = unpack($format, pack($format, $value))[1];
    if ($actual !== 42) {
        echo "Unexpected result for $format: ";
        var_dump($actual);
        $passed = false;
    }
}
echo "integer formats: ";
var_dump($passed);

$passed = true;
foreach ($floatFormats as $format) {
    $actual = unpack($format, pack($format, $value))[1];
    if ($actual !== 42.0) {
        echo "Unexpected result for $format: ";
        var_dump($actual);
        $passed = false;
    }
}
echo "floating-point formats: ";
var_dump($passed);

?>
--EXPECT--
integer formats: bool(true)
floating-point formats: bool(true)
