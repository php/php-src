--TEST--
Test implode() function: error conditions
--FILE--
<?php
/* only glue */
try {
    var_dump(implode("glue"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* NULL as pieces */
try {
    var_dump(implode("glue", NULL));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* integer as glue */
try {
    var_dump(implode(12, "pieces"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: implode(): If argument #1 ($separator) is of type string, argument #2 ($array) must be of type array, null given
TypeError: implode(): If argument #1 ($separator) is of type string, argument #2 ($array) must be of type array, null given
TypeError: implode(): Argument #2 ($array) must be of type ?array, string given
