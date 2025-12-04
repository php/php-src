--TEST--
Behavior of union type checks (strict)
--FILE--
<?php

declare(strict_types=1);

function dump($value) {
    if (is_object($value)) {
        return 'new ' . get_class($value);
    }
    if ($value === INF) {
        return 'INF';
    }
    return json_encode($value, JSON_PRESERVE_ZERO_FRACTION);
}

function test(string $type, array $values) {
    $alignment = 16;

    echo "\nType $type:\n";
    $fn = eval("return function($type \$arg) { return \$arg; };");
    foreach ($values as $value) {
        echo str_pad(dump($value), $alignment), ' => ';

        try {
            error_clear_last();
            $value = @$fn($value);
            echo dump($value);
            if ($e = error_get_last()) {
                echo ' (', $e['message'], ')';
            }
        } catch (TypeError $e) {
            $msg = $e->getMessage();
            $msg = strstr($msg, ', called in', true);
            $msg = str_replace('{closure}(): Argument #1 ($arg)', 'Argument ...', $msg);
            echo $msg;
        }
        echo "\n";
    }
}

class WithToString {
    public function __toString() {
        return "__toString()";
    }
}

$values = [
    42, 42.0, INF, "42", "42.0", "42x", "x", "",
    true, false, null, [], new stdClass, new WithToString,
];
test('int|float', $values);
test('int|float|false', $values);
test('int|float|bool', $values);
test('int|bool', $values);
test('int|string|null', $values);
test('string|bool', $values);
test('float|array', $values);
test('string|array', $values);
test('bool|array', $values);

?>
--EXPECTF--
Type int|float:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, non-numeric-string given
"42.0"           => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, non-numeric-string given
"42x"            => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, non-numeric-string given
"x"              => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, non-numeric-string given
""               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, empty-string given
true             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, bool given
false            => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, bool given
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, null given
[]               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, array given
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float, WithToString given

Type int|float|false:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, non-numeric-string given
"42.0"           => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, non-numeric-string given
"42x"            => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, non-numeric-string given
"x"              => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, non-numeric-string given
""               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, empty-string given
true             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, bool given
false            => false
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, null given
[]               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, array given
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|false, WithToString given

Type int|float|bool:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, non-numeric-string given
"42.0"           => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, non-numeric-string given
"42x"            => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, non-numeric-string given
"x"              => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, non-numeric-string given
""               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, empty-string given
true             => true
false            => false
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, null given
[]               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, array given
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|float|bool, WithToString given

Type int|bool:
42               => 42
42.0             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, float given
INF              => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, float given
"42"             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, non-numeric-string given
"42.0"           => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, non-numeric-string given
"42x"            => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, non-numeric-string given
"x"              => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, non-numeric-string given
""               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, empty-string given
true             => true
false            => false
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, null given
[]               => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, array given
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type int|bool, WithToString given

Type int|string|null:
42               => 42
42.0             => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, float given
INF              => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, float given
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, bool given
false            => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, bool given
null             => null
[]               => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, array given
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|int|null, WithToString given

Type string|bool:
42               => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, int given
42.0             => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, float given
INF              => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, float given
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => true
false            => false
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, null given
[]               => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, array given
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type string|bool, WithToString given

Type float|array:
42               => 42.0
42.0             => 42.0
INF              => INF
"42"             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, non-numeric-string given
"42.0"           => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, non-numeric-string given
"42x"            => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, non-numeric-string given
"x"              => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, non-numeric-string given
""               => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, empty-string given
true             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, bool given
false            => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, bool given
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, null given
[]               => []
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|float, WithToString given

Type string|array:
42               => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, int given
42.0             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, float given
INF              => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, float given
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, bool given
false            => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, bool given
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, null given
[]               => []
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|string, WithToString given

Type bool|array:
42               => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, int given
42.0             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, float given
INF              => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, float given
"42"             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, non-numeric-string given
"42.0"           => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, non-numeric-string given
"42x"            => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, non-numeric-string given
"x"              => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, non-numeric-string given
""               => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, empty-string given
true             => true
false            => false
null             => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, null given
[]               => []
new stdClass     => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, stdClass given
new WithToString => {closure:%s:%d}(): Argument #1 ($arg) must be of type array|bool, WithToString given
