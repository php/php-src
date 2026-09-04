--TEST--
pack() float and double value conversions
--FILE--
<?php

$values = [
    'null' => null,
    'false' => false,
    'true' => true,
    'int' => 42,
    'float' => 42.5,
    'numeric integer string' => '42',
    'numeric float string' => '42.5',
    'numeric scientific string' => '1e3',
];

foreach ($values as $name => $value) {
    echo "$name: ";
    var_dump(unpack('d', pack('d', $value))[1]);
}

echo "reference: ";
$value = 1.5;
$reference =& $value;
var_dump(unpack('d', pack('d', $reference))[1]);

echo "trailing data:\n";
var_dump(unpack('d', pack('d', '42 with trailing data'))[1]);

$invalidValues = [
    'empty string' => '',
    'whitespace string' => ' ',
    'non-numeric string' => 'not numeric',
    'array' => [],
    'object' => new stdClass(),
    'resource' => fopen(__FILE__, 'r'),
];

foreach ($invalidValues as $name => $value) {
    echo "$name:\n";
    try {
        pack('d', $value);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "all format codes:\n";
foreach (['f', 'g', 'G', 'd', 'e', 'E'] as $format) {
    try {
        pack($format, []);
    } catch (Throwable $e) {
        echo "$format: ", $e->getMessage(), "\n";
    }
}

echo "later argument:\n";
try {
    pack('d2', 1.0, []);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "warning converted to exception:\n";
set_error_handler(static function (int $errno, string $errstr): never {
    throw new Exception($errstr);
});
try {
    pack('d', '42 with trailing data');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
restore_error_handler();
fclose($invalidValues['resource']);

?>
--EXPECTF--
null: float(0)
false: float(0)
true: float(1)
int: float(42)
float: float(42.5)
numeric integer string: float(42)
numeric float string: float(42.5)
numeric scientific string: float(1000)
reference: float(1.5)
trailing data:

Warning: A non-numeric value encountered in %s on line %d
float(42)
empty string:
TypeError: pack(): Argument #2 must be of type float, string given
whitespace string:
TypeError: pack(): Argument #2 must be of type float, string given
non-numeric string:
TypeError: pack(): Argument #2 must be of type float, string given
array:
TypeError: pack(): Argument #2 must be of type float, array given
object:
TypeError: pack(): Argument #2 must be of type float, stdClass given
resource:
TypeError: pack(): Argument #2 must be of type float, resource given
all format codes:
f: pack(): Argument #2 must be of type float, array given
g: pack(): Argument #2 must be of type float, array given
G: pack(): Argument #2 must be of type float, array given
d: pack(): Argument #2 must be of type float, array given
e: pack(): Argument #2 must be of type float, array given
E: pack(): Argument #2 must be of type float, array given
later argument:
TypeError: pack(): Argument #3 must be of type float, array given
warning converted to exception:
Exception: A non-numeric value encountered
