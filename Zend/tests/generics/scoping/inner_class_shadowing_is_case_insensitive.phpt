--TEST--
Scoping: inner-class shadowing of a type parameter is case-insensitive
--FILE--
<?php
function f<T>(): void {
    class t {
        public int $v = 9;
    }
    $x = new T();
    echo $x->v, "\n";
}
f();
?>
--EXPECT--
9
