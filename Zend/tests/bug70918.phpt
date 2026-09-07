--TEST--
Bug #70918 (Segfault using static outside of class scope)
--FILE--
<?php
try {
    static::x;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    parent::x;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    self::x;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    new static;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    static::x();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    static::$i;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Cannot access "static" when no class scope is active
Error: Cannot access "parent" when no class scope is active
Error: Cannot access "self" when no class scope is active
Error: Cannot access "static" when no class scope is active
Error: Cannot access "static" when no class scope is active
Error: Cannot access "static" when no class scope is active
