--TEST--
new self / new parent in constant expression
--FILE--
<?php

class A {
    public static function invalid($x = new parent) {
    }
}
class B extends A {
    public static function method($x = new self, $y = new parent) {
        var_dump($x, $y);
    }
}

function invalid($x = new self) {}

B::method();

try {
    invalid();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    B::invalid();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(B)#1 (0) {
}
object(A)#2 (0) {
}
Cannot access "self" when no class scope is active
Cannot access "parent" when current class scope has no parent
