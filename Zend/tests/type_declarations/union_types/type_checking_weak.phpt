--TEST--
Behavior of union type checks (weak)
--FILE--
<?php

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
"42"             => 42
"42.0"           => 42.0
"42x"            => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, string given
"x"              => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, string given
""               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, string given
true             => 1 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type int|float is deprecated)
false            => 0 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type int|float is deprecated)
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, null given
[]               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, array given
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, stdClass given
new WithToString => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float, WithToString given

Type int|float|false:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => 42
"42.0"           => 42.0
"42x"            => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, string given
"x"              => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, string given
""               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, string given
true             => 1 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type int|float|false is deprecated)
false            => false
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, null given
[]               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, array given
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, stdClass given
new WithToString => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|false, WithToString given

Type int|float|bool:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => 42
"42.0"           => 42.0
"42x"            => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type int|float|bool is deprecated)
"x"              => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type int|float|bool is deprecated)
""               => false ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type int|float|bool is deprecated)
true             => true
false            => false
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|bool, null given
[]               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|bool, array given
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|bool, stdClass given
new WithToString => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|float|bool, WithToString given

Type int|bool:
42               => 42
42.0             => 42
INF              => true ({%s(%d) : eval()'d code:1}(): Passing float to parameter #1 ($arg) of type int|bool is deprecated)
"42"             => 42
"42.0"           => 42
"42x"            => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type int|bool is deprecated)
"x"              => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type int|bool is deprecated)
""               => false ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type int|bool is deprecated)
true             => true
false            => false
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|bool, null given
[]               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|bool, array given
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|bool, stdClass given
new WithToString => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type int|bool, WithToString given

Type int|string|null:
42               => 42
42.0             => 42
INF              => "INF"
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => 1 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type string|int|null is deprecated)
false            => 0 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type string|int|null is deprecated)
null             => null
[]               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type string|int|null, array given
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type string|int|null, stdClass given
new WithToString => "__toString()"

Type string|bool:
42               => "42"
42.0             => "42"
INF              => "INF"
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => true
false            => false
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type string|bool, null given
[]               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type string|bool, array given
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type string|bool, stdClass given
new WithToString => "__toString()"

Type float|array:
42               => 42.0
42.0             => 42.0
INF              => INF
"42"             => 42.0
"42.0"           => 42.0
"42x"            => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|float, string given
"x"              => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|float, string given
""               => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|float, string given
true             => 1.0 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type array|float is deprecated)
false            => 0.0 ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type array|float is deprecated)
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|float, null given
[]               => []
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|float, stdClass given
new WithToString => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|float, WithToString given

Type string|array:
42               => "42"
42.0             => "42"
INF              => "INF"
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => "1" ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type array|string is deprecated)
false            => "" ({%s(%d) : eval()'d code:1}(): Passing bool to parameter #1 ($arg) of type array|string is deprecated)
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|string, null given
[]               => []
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|string, stdClass given
new WithToString => "__toString()"

Type bool|array:
42               => true ({%s(%d) : eval()'d code:1}(): Passing int to parameter #1 ($arg) of type array|bool is deprecated)
42.0             => true ({%s(%d) : eval()'d code:1}(): Passing float to parameter #1 ($arg) of type array|bool is deprecated)
INF              => true ({%s(%d) : eval()'d code:1}(): Passing float to parameter #1 ($arg) of type array|bool is deprecated)
"42"             => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type array|bool is deprecated)
"42.0"           => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type array|bool is deprecated)
"42x"            => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type array|bool is deprecated)
"x"              => true ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type array|bool is deprecated)
""               => false ({%s(%d) : eval()'d code:1}(): Passing string to parameter #1 ($arg) of type array|bool is deprecated)
true             => true
false            => false
null             => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|bool, null given
[]               => []
new stdClass     => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|bool, stdClass given
new WithToString => {%s(%d) : eval()'d code:1}(): Argument #1 ($arg) must be of type array|bool, WithToString given
