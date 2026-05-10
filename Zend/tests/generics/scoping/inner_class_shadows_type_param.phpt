--TEST--
Scoping: a class declared inside a generic body shadows the type parameter
--FILE--
<?php
function f<T>(): void {
    class T {
        public int $v = 7;
    }
    $x = new T();
    echo $x->v, "\n";
}
f();
?>
--EXPECT--
7
