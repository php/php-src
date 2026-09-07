--TEST--
array_intersect() conversion side effects and argument validation
--FILE--
<?php
ini_set('precision', '14');

class StringableWithStatefulDestructor {
    public function __toString(): string {
        return 'unmatched';
    }

    public function __destruct() {
        ini_set('precision', '3');
    }
}

function temporary_values(): array {
    return [new StringableWithStatefulDestructor(), 1.234567];
}

var_dump(array_intersect(temporary_values(), ['1.234567']));

class CapturingStringableValue {
    public static array $argument;

    public function __toString(): string {
        self::$argument = debug_backtrace()[1]['args'][0];
        return 'drop';
    }
}

function temporary_argument(): array {
    return [
        'drop' => new CapturingStringableValue(),
        'keep' => 'keep',
    ];
}

$result = array_intersect(temporary_argument(), ['keep']);
var_dump(array_keys($result), array_keys(CapturingStringableValue::$argument));

set_error_handler(static function (int $code, string $message): bool {
    echo $message, "\n";
    return true;
});

try {
    array_intersect([[1], [2]], ['Array'], new stdClass());
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

restore_error_handler();
?>
--EXPECT--
array(1) {
  [1]=>
  float(1.234567)
}
array(1) {
  [0]=>
  string(4) "keep"
}
array(2) {
  [0]=>
  string(4) "drop"
  [1]=>
  string(4) "keep"
}
TypeError: array_intersect(): Argument #3 must be of type array, stdClass given
