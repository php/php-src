--TEST--
Scoping: method type parameter does not leak to other methods
--FILE--
<?php
class T {
    public int $val = 7;
}
class Box {
    public function m1<U : object>(): int {
        return 1;
    }
    public function m2(): T {
        return new T;
    }
}
$b = new Box;
echo $b->m2()->val, "\n";
?>
--EXPECT--
7
