--TEST--
Test ResourceBundle errors with []
--EXTENSIONS--
intl
--FILE--
<?php
include "resourcebundle.inc";

// fall back
$r = new ResourceBundle( 'en_US', BUNDLE );

try {
    $ref = &$r[];
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(isset($r['non-existent']));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($r['non-existent'] ?? "default");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($r[12.5]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($r[new stdClass()]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($r['']);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    /* This can only happen on 64bit */
    if (PHP_INT_SIZE > 4) {
        var_dump($r[0xFFFFFFFFF]);
    } else {
        echo 'ValueError: Index must be between -2147483648 and 2147483647', PHP_EOL;
    }
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot apply [] to ResourceBundle object
Error: Cannot use object of type ResourceBundle as array
string(7) "default"
TypeError: Cannot access offset of type float on ResourceBundle
TypeError: Cannot access offset of type stdClass on ResourceBundle
ValueError: Offset cannot be empty
ValueError: Index must be between -2147483648 and 2147483647
