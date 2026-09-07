--TEST--
array_intersect() does not convert values when an argument is empty
--FILE--
<?php
class ThrowingStringableValue {
    public function __toString(): string {
        throw new RuntimeException('conversion failed');
    }
}

set_error_handler(static function (int $code, string $message): never {
    throw new ErrorException($message, $code);
});

$cases = [
    static fn() => array_intersect([], [[1]]),
    static fn() => array_intersect([[1]], []),
    static fn() => array_intersect([new ThrowingStringableValue()], ['value'], []),
    static fn() => array_intersect([], [new stdClass()]),
];

foreach ($cases as $case) {
    try {
        var_dump($case());
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

restore_error_handler();

$result = array_intersect([9 => 'value'], []);
$result[] = 'appended';
var_dump(array_keys($result));

try {
    array_intersect([], [], new stdClass());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  int(10)
}
TypeError: array_intersect(): Argument #3 must be of type array, stdClass given
