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
string(43) ""static" cannot be used in the global scope"
string(43) ""parent" cannot be used in the global scope"
string(41) ""self" cannot be used in the global scope"
string(43) ""static" cannot be used in the global scope"
string(43) ""static" cannot be used in the global scope"
string(43) ""static" cannot be used in the global scope"
