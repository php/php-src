--TEST--
InvalidOperatorError thrown for unsupported arithmetic operands on Operable while non-Operable and bitwise ops keep plain TypeError
--FILE--
<?php

final readonly class Money implements Operable {
    public function __add(self $other): self {
        return $this;
    }
}

class NonOperable {
    public function __add($other) {
        return "should never be called";
    }
}

$m = new Money();
$n = new NonOperable();

// Operable unsupported arithmetic operand throws InvalidOperatorError (subclass of TypeError)
$operableCases = [
    fn() => $m + "100",
    fn() => "100" + $m,
    fn() => $m - 1,
    fn() => $m * 2,
    fn() => $m / 2,
    fn() => $m % 2,
    fn() => $m ** 2,
];

foreach ($operableCases as $fn) {
    try {
        $fn();
    } catch (TypeError $e) {
        echo get_class($e) . " (is InvalidOperatorError=" . ($e instanceof InvalidOperatorError ? "yes" : "no") . "): " . $e->getMessage() . "\n";
    }
}

// Non-Operable arithmetic ops and bitwise/shift ops still throw plain TypeError (not InvalidOperatorError)
$plainTypeErrorCases = [
    fn() => $n + 1,
    fn() => [] + 1,
    fn() => $m | 1,
    fn() => $m & 1,
    fn() => $m ^ 1,
    fn() => $m << 1,
    fn() => $m >> 1,
];

foreach ($plainTypeErrorCases as $fn) {
    try {
        $fn();
    } catch (TypeError $e) {
        echo get_class($e) . " (is InvalidOperatorError=" . ($e instanceof InvalidOperatorError ? "yes" : "no") . "): " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: Money + string
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: string + Money
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: Money - int
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: Money * int
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: Money / int
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: Money % int
InvalidOperatorError (is InvalidOperatorError=yes): Unsupported operand types: Money ** int
TypeError (is InvalidOperatorError=no): Unsupported operand types: NonOperable + int
TypeError (is InvalidOperatorError=no): Unsupported operand types: array + int
TypeError (is InvalidOperatorError=no): Unsupported operand types: Money | int
TypeError (is InvalidOperatorError=no): Unsupported operand types: Money & int
TypeError (is InvalidOperatorError=no): Unsupported operand types: Money ^ int
TypeError (is InvalidOperatorError=no): Unsupported operand types: Money << int
TypeError (is InvalidOperatorError=no): Unsupported operand types: Money >> int
