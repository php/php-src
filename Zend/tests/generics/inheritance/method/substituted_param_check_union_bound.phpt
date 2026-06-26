--TEST--
Inherited methods: substituted parameter type is enforced when the parent's bound is a union
--FILE--
<?php
class Box<T : int|string> {
    public function take(T $v): void {}
}

class IntBox extends Box<int> {}
class StringBox extends Box<string> {}

// Substituted IntBox rejects non-numeric string even though parent's bound allows it.
try {
    (new IntBox())->take("hello");
} catch (TypeError $e) {
    echo "int-str: ", $e->getMessage(), "\n";
}

// Substituted IntBox rejects array (not in parent's bound either, but check the message).
try {
    (new IntBox())->take([1]);
} catch (TypeError $e) {
    echo "int-arr: ", $e->getMessage(), "\n";
}

// Substituted StringBox rejects array.
try {
    (new StringBox())->take([1]);
} catch (TypeError $e) {
    echo "str-arr: ", $e->getMessage(), "\n";
}

// Parent's bound accepts both int and string.
(new Box())->take(42);
(new Box())->take("hello");
echo "parent ok\n";
?>
--EXPECTF--
int-str: Box::take(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
int-arr: Box::take(): Argument #1 ($v) must be of type int, array given, called in %s on line %d
str-arr: Box::take(): Argument #1 ($v) must be of type string, array given, called in %s on line %d
parent ok
