--TEST--
Errors thrown by namespace_declare()
--FILE--
<?php

$args = [
    ["", []],
    ["\\Foo", []],
    ["Foo\\", []],
    ["Foo", ["abc"]],
    ["Foo", ["encoding" => "utf-8"]],
    ["Foo", ["ticks" => 12.7]],
    ["Foo", ["ticks" => -1]],
    ["Foo", ["strict_types" => "on"]],
    ["Foo", ["strict_types" => -1]],
];

foreach ($args as [$namespace, $declares]) {
    try {
        namespace_declare($namespace, $declares);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

namespace_declare("Foo", ["ticks" => 0]);
try {
    namespace_declare("Foo", ["ticks" => 0]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Namespace cannot be empty
Namespace cannot start with "\"
Namespace cannot end with "\"
Declare directive array must have string keys
Cannot use "encoding" in namespace declare
Value of "ticks" must be a non-negative integer
Value of "ticks" must be a non-negative integer
Value of "strict_types" must be 0 or 1
Value of "strict_types" must be 0 or 1
Declares for namespace "Foo" already defined
