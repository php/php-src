--TEST--
Typed class constants (type coercion is unsupported)
--FILE--
<?php
class S {
    public function __toString() {
        echo "Side effect!\n";
        return 'S';
    }
}

class A {
    public const string S = S;
}

define("S", new S());

try {
    var_dump(A::S);
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Cannot assign S to class constant A::S of type string
