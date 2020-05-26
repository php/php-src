--TEST--
Cannot access self::class when no class scope is active
--FILE--
<?php

try {
    var_dump(self::class);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump([self::class]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot use "self" in the global scope
Cannot use "self" in the global scope
