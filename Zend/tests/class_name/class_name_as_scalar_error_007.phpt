--TEST--
Cannot access self::class when no class scope is active
--FILE--
<?php

try {
    var_dump(self::class);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump([self::class]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot use "self" in the global scope
Error: Cannot use "self" in the global scope
