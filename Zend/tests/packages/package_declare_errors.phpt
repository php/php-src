--TEST--
Error conditions of package_declare()
--FILE--
<?php

try {
    package_declare([]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => 42,
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => "foo/bar",
        "declares" => 42,
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => "foo/bar",
        "declares" => [
            "foo"
        ],
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => "foo/bar",
        "declares" => [
            "ticks" => "foo",
        ],
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => "foo/bar",
        "declares" => [
            "ticks" => -1,
        ],
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => "foo/bar",
        "declares" => [
            "strict_types" => "foo",
        ],
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    package_declare([
        "name" => "foo/bar",
        "declares" => [
            "strict_types" => 42,
        ],
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

// Allowed for forward-compatibility
package_declare([
    "name" => "foo/bar",
    "declares" => [
        "unknown" => "foo",
    ],
]);


try {
    package_declare([
        "name" => "foo/bar",
    ]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Package specification must contain "name" key of type string
Package specification must contain "name" key of type string
Package specification key "declares" must be of type array
Declares array in package specification must have string keys
Value of "ticks" must be a non-negative integer
Value of "ticks" must be a non-negative integer
Value of "strict_types" must be 0 or 1
Value of "strict_types" must be 0 or 1
Package "foo/bar" already registered
