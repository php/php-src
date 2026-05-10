--TEST--
Inherited methods: non-generic parameters keep their normal RECV fast-path even on substituted children
--FILE--
<?php
class Box<T> {
    // First parameter is plain int (not T); second is T.
    public function mixed(int $count, T $value): void {
        echo "count=$count value=";
        var_dump($value);
    }
}

class IntBox extends Box<int> {}

$b = new IntBox();

// Plain int param accepts int, coerces numeric string in weak mode.
$b->mixed(3, 42);
$b->mixed("7", 9);

// Plain int param still rejects non-numeric string.
try {
    $b->mixed("hello", 1);
} catch (TypeError $e) {
    echo "plain-int: ", $e->getMessage(), "\n";
}

// Substituted T param rejects string.
try {
    $b->mixed(1, "hello");
} catch (TypeError $e) {
    echo "subst-T: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
count=3 value=int(42)
count=7 value=int(9)
plain-int: Box::mixed(): Argument #1 ($count) must be of type int, string given, called in %s on line %d
subst-T: Box::mixed(): Argument #2 ($value) must be of type int, string given, called in %s on line %d
