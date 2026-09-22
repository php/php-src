--TEST--
array_intersect() with all value types
--FILE--
<?php
function dump_array(array $value): void {
    echo json_encode($value), "\n";
}

dump_array(array_intersect(
    ['first' => 1, 'duplicate' => 1, 'leading' => '01', 'plus' => '+1', 'exponent' => '1e0', 'zero' => 0, 'negative-zero' => '-0'],
    ['1', '0'],
));

dump_array(array_intersect([1, 2, '2', 3], [1, 2, 2, '2', 3], [2, '2', 3]));

$aboveMax = PHP_INT_SIZE === 8 ? "9223372036854775808" : "2147483648";
$belowMin = PHP_INT_SIZE === 8 ? "-9223372036854775809" : "-2147483649";
var_dump(
    array_intersect([PHP_INT_MAX, PHP_INT_MIN], [(string) PHP_INT_MAX, (string) PHP_INT_MIN]) === [PHP_INT_MAX, PHP_INT_MIN],
    array_intersect([PHP_INT_MAX, PHP_INT_MIN], [$aboveMax, $belowMin]) === [],
    array_intersect([$aboveMax, $belowMin], [$aboveMax, $belowMin]) === [$aboveMax, $belowMin],
);

$integer = 2;
$string = '2';
$result = array_intersect([&$integer, 3], [&$string, 3]);
$integer = 9;
var_dump($result[0]);

$array = [0 => 'drop', 100 => 'x', 200 => 'y'];
$result = array_intersect($array, ['x', 'y']);
$expected = $array;
unset($expected[0]);
mt_srand(0);
$resultRandom = [array_rand($result), mt_rand()];
mt_srand(0);
$expectedRandom = [array_rand($expected), mt_rand()];
var_dump($result === $expected, $resultRandom === $expectedRandom);

class StringableValue {
    public function __construct(private string $value) {}
    public function __toString(): string { return $this->value; }
}

$resource = fopen('php://memory', 'r');
$result = array_intersect(
    [
        'null' => null,
        'false' => false,
        'true' => true,
        'zero-float' => 0.0,
        'float' => 1.5,
        'resource' => $resource,
        'object' => new StringableValue('object'),
    ],
    ['', '1', '0', '1.5', (string) $resource, 'object'],
    [false, true, 0, '1.5', $resource, new StringableValue('object')],
);
echo implode(',', array_keys($result)), "\n";

var_dump(@array_intersect([[1]], [[2]]) === [[1]]);

class ThrowingStringableValue {
    public function __toString(): string { throw new RuntimeException('conversion failed'); }
}

try {
    array_intersect(['value'], [new ThrowingStringableValue(), 'value']);
} catch (RuntimeException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    array_intersect([''], [new stdClass()]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

set_error_handler(static function (int $code, string $message): never {
    throw new ErrorException($message, $code);
});
try {
    array_intersect([[1]], [[2]]);
} catch (ErrorException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
} finally {
    restore_error_handler();
}
?>
--EXPECT--
{"first":1,"duplicate":1,"zero":0}
{"1":2,"2":"2","3":3}
bool(true)
bool(true)
bool(true)
int(9)
bool(true)
bool(true)
null,false,true,zero-float,float,resource,object
bool(true)
RuntimeException: conversion failed
Error: Object of class stdClass could not be converted to string
ErrorException: Array to string conversion
