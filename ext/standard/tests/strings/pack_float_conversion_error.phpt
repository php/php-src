--TEST--
pack() float and double value conversion errors
--FILE--
<?php

$formats = ['f', 'g', 'G', 'd', 'e', 'E'];
$resource = fopen(__FILE__, 'r');
$invalidValues = [
    'empty string' => ['', 'string'],
    'whitespace string' => [' ', 'string'],
    'non-numeric string' => ['not numeric', 'string'],
    'INF string' => ['INF', 'string'],
    'NAN string' => ['NAN', 'string'],
    'empty array' => [[], 'array'],
    'non-empty array' => [[1], 'array'],
    'object' => [new stdClass(), 'stdClass'],
    'resource' => [$resource, 'resource'],
];

foreach ($formats as $format) {
    echo "$format:\n";
    foreach ($invalidValues as $name => [$value, $type]) {
        echo "$name: ";
        try {
            pack($format, $value);
            echo "accepted\n";
        } catch (Throwable $e) {
            $expected = "pack(): Argument #2 must be of type float for format code '$format', $type given";
            var_dump($e instanceof TypeError && $e->getMessage() === $expected);
        }
    }
}

echo "invalid reference:\n";
$value = [];
$reference =& $value;
foreach ($formats as $format) {
    echo "$format: ";
    try {
        pack($format, $reference);
        echo "accepted\n";
    } catch (Throwable $e) {
        $expected = "pack(): Argument #2 must be of type float for format code '$format', array given";
        var_dump($e instanceof TypeError && $e->getMessage() === $expected);
    }
}

echo "argument numbers:\n";
$cases = [
    ['f2', [1.0, []], 3, 'f'],
    ['d*', [1.0, []], 3, 'd'],
    ['C2g', [1, 2, []], 4, 'g'],
    ['f2d2', [1.0, 2.0, 3.0, []], 5, 'd'],
];
foreach ($cases as [$format, $arguments, $argumentNumber, $valueFormat]) {
    echo "$format: ";
    try {
        pack($format, ...$arguments);
        echo "accepted\n";
    } catch (Throwable $e) {
        $expected = "pack(): Argument #$argumentNumber must be of type float "
            . "for format code '$valueFormat', array given";
        var_dump($e instanceof TypeError && $e->getMessage() === $expected);
    }
}

echo "warning converted to exception:\n";
set_error_handler(static function (int $errno, string $errstr): never {
    throw new Exception($errstr);
});
foreach ($formats as $format) {
    try {
        pack($format, '42 with trailing data');
        echo "$format: accepted\n";
    } catch (Throwable $e) {
        echo "$format: ", $e::class, ': ', $e->getMessage(), "\n";
    }
}
restore_error_handler();
fclose($resource);

?>
--EXPECT--
f:
empty string: bool(true)
whitespace string: bool(true)
non-numeric string: bool(true)
INF string: bool(true)
NAN string: bool(true)
empty array: bool(true)
non-empty array: bool(true)
object: bool(true)
resource: bool(true)
g:
empty string: bool(true)
whitespace string: bool(true)
non-numeric string: bool(true)
INF string: bool(true)
NAN string: bool(true)
empty array: bool(true)
non-empty array: bool(true)
object: bool(true)
resource: bool(true)
G:
empty string: bool(true)
whitespace string: bool(true)
non-numeric string: bool(true)
INF string: bool(true)
NAN string: bool(true)
empty array: bool(true)
non-empty array: bool(true)
object: bool(true)
resource: bool(true)
d:
empty string: bool(true)
whitespace string: bool(true)
non-numeric string: bool(true)
INF string: bool(true)
NAN string: bool(true)
empty array: bool(true)
non-empty array: bool(true)
object: bool(true)
resource: bool(true)
e:
empty string: bool(true)
whitespace string: bool(true)
non-numeric string: bool(true)
INF string: bool(true)
NAN string: bool(true)
empty array: bool(true)
non-empty array: bool(true)
object: bool(true)
resource: bool(true)
E:
empty string: bool(true)
whitespace string: bool(true)
non-numeric string: bool(true)
INF string: bool(true)
NAN string: bool(true)
empty array: bool(true)
non-empty array: bool(true)
object: bool(true)
resource: bool(true)
invalid reference:
f: bool(true)
g: bool(true)
G: bool(true)
d: bool(true)
e: bool(true)
E: bool(true)
argument numbers:
f2: bool(true)
d*: bool(true)
C2g: bool(true)
f2d2: bool(true)
warning converted to exception:
f: Exception: A non-numeric value encountered
g: Exception: A non-numeric value encountered
G: Exception: A non-numeric value encountered
d: Exception: A non-numeric value encountered
e: Exception: A non-numeric value encountered
E: Exception: A non-numeric value encountered
