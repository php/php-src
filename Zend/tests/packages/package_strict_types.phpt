--TEST--
Specifying strict_types through package declaration
--FILE--
<?php

package_declare([
    "name" => "with_strict_types",
    "declares" => [
        "strict_types" => 1,
    ],
]);

eval(<<<'PHP'
// Package defaults to strict types
package "with_strict_types";

try {
    var_dump(strlen(42));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
PHP);

eval(<<<'PHP'
// Manual override to strict_types=0
package "with_strict_types";
declare(strict_types=0);

try {
    var_dump(strlen(42));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
PHP);

eval(<<<'PHP'
// No package, no strict_types

try {
    var_dump(strlen(42));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
PHP);

?>
--EXPECT--
strlen() expects parameter 1 to be string, int given
int(2)
int(2)
