--TEST--
Bug #70918 (Segfault using static outside of class scope)
--FILE--
<?php
try {
    static::x;
} catch (Error $e) {
    var_dump($e->getMessage());
}

try {
    parent::x;
} catch (Error $e) {
    var_dump($e->getMessage());
}

try {
    self::x;
} catch (Error $e) {
    var_dump($e->getMessage());
}

try {
    new static;
} catch (Error $e) {
    var_dump($e->getMessage());
}

try {
    static::x();
} catch (Error $e) {
    var_dump($e->getMessage());
}

try {
    static::$i;
} catch (Error $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(52) "Cannot access "static" when no class scope is active"
string(52) "Cannot access "parent" when no class scope is active"
string(50) "Cannot access "self" when no class scope is active"
string(52) "Cannot access "static" when no class scope is active"
string(52) "Cannot access "static" when no class scope is active"
string(52) "Cannot access "static" when no class scope is active"
