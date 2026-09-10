--TEST--
pack() float and double value conversions
--FILE--
<?php

$formats = ['f', 'g', 'G', 'd', 'e', 'E'];
$values = [
    'null' => [null, 0.0],
    'false' => [false, 0.0],
    'true' => [true, 1.0],
    'int' => [42, 42.0],
    'float' => [42.5, 42.5],
    'numeric integer string' => ['42', 42.0],
    'numeric float string' => ['42.5', 42.5],
    'numeric scientific string' => ['1e3', 1000.0],
    'leading whitespace' => [' 42.5', 42.5],
    'trailing whitespace' => ["42.5 \t", 42.5],
];

foreach ($formats as $format) {
    echo "$format:\n";
    foreach ($values as $name => [$value, $expected]) {
        $actual = unpack($format, pack($format, $value))[1];
        echo "$name: ";
        var_dump($actual === $expected);
    }
}

echo "references:\n";
$value = 1.5;
$reference =& $value;
foreach ($formats as $format) {
    $actual = unpack($format, pack($format, $reference))[1];
    echo "$format: ";
    var_dump($actual === 1.5);
}

echo "special float values:\n";
foreach ($formats as $format) {
    $infinity = unpack($format, pack($format, INF))[1];
    $nan = unpack($format, pack($format, NAN))[1];
    echo "$format: ";
    var_dump($infinity === INF && is_nan($nan));
}

echo "signed zero integer strings:\n";
foreach ($formats as $format) {
    $actual = unpack($format, pack($format, '-0'))[1];
    echo "$format: ";
    var_dump($actual === 0.0 && fdiv(1.0, $actual) === -INF);
}

echo "leading-numeric strings:\n";
foreach ($formats as $format) {
    $diagnostic = null;
    set_error_handler(static function (int $errno, string $errstr) use (&$diagnostic): bool {
        $diagnostic = [$errno, $errstr];
        return true;
    });
    $actual = unpack($format, pack($format, '42 with trailing data'))[1];
    restore_error_handler();

    echo "$format: ";
    var_dump($diagnostic === [E_WARNING, 'A non-numeric value encountered'] && $actual === 42.0);
}

?>
--EXPECT--
f:
null: bool(true)
false: bool(true)
true: bool(true)
int: bool(true)
float: bool(true)
numeric integer string: bool(true)
numeric float string: bool(true)
numeric scientific string: bool(true)
leading whitespace: bool(true)
trailing whitespace: bool(true)
g:
null: bool(true)
false: bool(true)
true: bool(true)
int: bool(true)
float: bool(true)
numeric integer string: bool(true)
numeric float string: bool(true)
numeric scientific string: bool(true)
leading whitespace: bool(true)
trailing whitespace: bool(true)
G:
null: bool(true)
false: bool(true)
true: bool(true)
int: bool(true)
float: bool(true)
numeric integer string: bool(true)
numeric float string: bool(true)
numeric scientific string: bool(true)
leading whitespace: bool(true)
trailing whitespace: bool(true)
d:
null: bool(true)
false: bool(true)
true: bool(true)
int: bool(true)
float: bool(true)
numeric integer string: bool(true)
numeric float string: bool(true)
numeric scientific string: bool(true)
leading whitespace: bool(true)
trailing whitespace: bool(true)
e:
null: bool(true)
false: bool(true)
true: bool(true)
int: bool(true)
float: bool(true)
numeric integer string: bool(true)
numeric float string: bool(true)
numeric scientific string: bool(true)
leading whitespace: bool(true)
trailing whitespace: bool(true)
E:
null: bool(true)
false: bool(true)
true: bool(true)
int: bool(true)
float: bool(true)
numeric integer string: bool(true)
numeric float string: bool(true)
numeric scientific string: bool(true)
leading whitespace: bool(true)
trailing whitespace: bool(true)
references:
f: bool(true)
g: bool(true)
G: bool(true)
d: bool(true)
e: bool(true)
E: bool(true)
special float values:
f: bool(true)
g: bool(true)
G: bool(true)
d: bool(true)
e: bool(true)
E: bool(true)
signed zero integer strings:
f: bool(true)
g: bool(true)
G: bool(true)
d: bool(true)
e: bool(true)
E: bool(true)
leading-numeric strings:
f: bool(true)
g: bool(true)
G: bool(true)
d: bool(true)
e: bool(true)
E: bool(true)
