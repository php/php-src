--TEST--
Error during evaluation of static/constants
--FILE--
<?php

try {
    class A {
        public const C = 42;
        public static $s1 = UNKNOWN;
        public static $s2 = 42;
    }
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(A::$s2);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new A);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new A);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    // TODO: Should this fail?
    var_dump(A::C);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Undefined constant "UNKNOWN"
Trying to use class for which initializer evaluation has previously failed
Trying to use class for which initializer evaluation has previously failed
Trying to use class for which initializer evaluation has previously failed
int(42)
