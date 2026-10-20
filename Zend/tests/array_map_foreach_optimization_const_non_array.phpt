--TEST--
array_map(): foreach optimization - constant non-array argument does not leak
--FILE--
<?php

$tests = [
    'string' => fn() => array_map(strtoupper(...), "str"),
    'int' => fn() => array_map(strtoupper(...), 1),
    'float' => fn() => array_map(strtoupper(...), 1.5),
    'true' => fn() => array_map(strtoupper(...), true),
    'null' => fn() => array_map(strtoupper(...), null),
    'folded string' => fn() => array_map(strtoupper(...), "a" . "b"),
    'static call' => fn() => array_map(Foo::bar(...), "str"),
];

foreach ($tests as $name => $test) {
    try {
        $test();
    } catch (Throwable $e) {
        echo $name, ': ', get_class($e), ': ', $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
string: TypeError: array_map(): Argument #2 ($array) must be of type array, string given
int: TypeError: array_map(): Argument #2 ($array) must be of type array, int given
float: TypeError: array_map(): Argument #2 ($array) must be of type array, float given
true: TypeError: array_map(): Argument #2 ($array) must be of type array, true given
null: TypeError: array_map(): Argument #2 ($array) must be of type array, null given
folded string: TypeError: array_map(): Argument #2 ($array) must be of type array, string given
static call: Error: Class "Foo" not found
