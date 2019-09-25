--TEST--
Behavior of union type checks (strict)
--SKIPIF--
<?php
if (!extension_loaded('json')) die('skip requires json');
?>
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
            $msg = str_replace('1 passed to {closure}()', '...', $msg);
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
--EXPECT--
Type int|float:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => Argument ... must be of type int|float, string given
"42.0"           => Argument ... must be of type int|float, string given
"42x"            => Argument ... must be of type int|float, string given
"x"              => Argument ... must be of type int|float, string given
""               => Argument ... must be of type int|float, string given
true             => Argument ... must be of type int|float, bool given
false            => Argument ... must be of type int|float, bool given
null             => Argument ... must be of type int|float, null given
[]               => Argument ... must be of type int|float, array given
new stdClass     => Argument ... must be of type int|float, object given
new WithToString => Argument ... must be of type int|float, object given

Type int|float|false:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => Argument ... must be of type int|float|false, string given
"42.0"           => Argument ... must be of type int|float|false, string given
"42x"            => Argument ... must be of type int|float|false, string given
"x"              => Argument ... must be of type int|float|false, string given
""               => Argument ... must be of type int|float|false, string given
true             => Argument ... must be of type int|float|false, bool given
false            => false
null             => Argument ... must be of type int|float|false, null given
[]               => Argument ... must be of type int|float|false, array given
new stdClass     => Argument ... must be of type int|float|false, object given
new WithToString => Argument ... must be of type int|float|false, object given

Type int|float|bool:
42               => 42
42.0             => 42.0
INF              => INF
"42"             => Argument ... must be of type int|float|bool, string given
"42.0"           => Argument ... must be of type int|float|bool, string given
"42x"            => Argument ... must be of type int|float|bool, string given
"x"              => Argument ... must be of type int|float|bool, string given
""               => Argument ... must be of type int|float|bool, string given
true             => true
false            => false
null             => Argument ... must be of type int|float|bool, null given
[]               => Argument ... must be of type int|float|bool, array given
new stdClass     => Argument ... must be of type int|float|bool, object given
new WithToString => Argument ... must be of type int|float|bool, object given

Type int|bool:
42               => 42
42.0             => Argument ... must be of type int|bool, float given
INF              => Argument ... must be of type int|bool, float given
"42"             => Argument ... must be of type int|bool, string given
"42.0"           => Argument ... must be of type int|bool, string given
"42x"            => Argument ... must be of type int|bool, string given
"x"              => Argument ... must be of type int|bool, string given
""               => Argument ... must be of type int|bool, string given
true             => true
false            => false
null             => Argument ... must be of type int|bool, null given
[]               => Argument ... must be of type int|bool, array given
new stdClass     => Argument ... must be of type int|bool, object given
new WithToString => Argument ... must be of type int|bool, object given

Type int|string|null:
42               => 42
42.0             => Argument ... must be of type string|int|null, float given
INF              => Argument ... must be of type string|int|null, float given
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => Argument ... must be of type string|int|null, bool given
false            => Argument ... must be of type string|int|null, bool given
null             => null
[]               => Argument ... must be of type string|int|null, array given
new stdClass     => Argument ... must be of type string|int|null, object given
new WithToString => Argument ... must be of type string|int|null, object given

Type string|bool:
42               => Argument ... must be of type string|bool, int given
42.0             => Argument ... must be of type string|bool, float given
INF              => Argument ... must be of type string|bool, float given
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => true
false            => false
null             => Argument ... must be of type string|bool, null given
[]               => Argument ... must be of type string|bool, array given
new stdClass     => Argument ... must be of type string|bool, object given
new WithToString => Argument ... must be of type string|bool, object given

Type float|array:
42               => 42.0
42.0             => 42.0
INF              => INF
"42"             => Argument ... must be of type array|float, string given
"42.0"           => Argument ... must be of type array|float, string given
"42x"            => Argument ... must be of type array|float, string given
"x"              => Argument ... must be of type array|float, string given
""               => Argument ... must be of type array|float, string given
true             => Argument ... must be of type array|float, bool given
false            => Argument ... must be of type array|float, bool given
null             => Argument ... must be of type array|float, null given
[]               => []
new stdClass     => Argument ... must be of type array|float, object given
new WithToString => Argument ... must be of type array|float, object given

Type string|array:
42               => Argument ... must be of type array|string, int given
42.0             => Argument ... must be of type array|string, float given
INF              => Argument ... must be of type array|string, float given
"42"             => "42"
"42.0"           => "42.0"
"42x"            => "42x"
"x"              => "x"
""               => ""
true             => Argument ... must be of type array|string, bool given
false            => Argument ... must be of type array|string, bool given
null             => Argument ... must be of type array|string, null given
[]               => []
new stdClass     => Argument ... must be of type array|string, object given
new WithToString => Argument ... must be of type array|string, object given

Type bool|array:
42               => Argument ... must be of type array|bool, int given
42.0             => Argument ... must be of type array|bool, float given
INF              => Argument ... must be of type array|bool, float given
"42"             => Argument ... must be of type array|bool, string given
"42.0"           => Argument ... must be of type array|bool, string given
"42x"            => Argument ... must be of type array|bool, string given
"x"              => Argument ... must be of type array|bool, string given
""               => Argument ... must be of type array|bool, string given
true             => true
false            => false
null             => Argument ... must be of type array|bool, null given
[]               => []
new stdClass     => Argument ... must be of type array|bool, object given
new WithToString => Argument ... must be of type array|bool, object given
