--TEST--
Recursive bounds: a class with mutually recursive bounds can be instantiated and used
--FILE--
<?php
class Box<T> {
    public function __construct(public mixed $value) {}
}

final class Pair<T: Box<U>, U: Box<T>> {
    public function __construct(public Box $left, public Box $right) {}
}

$bx = new Box(1);
$by = new Box("hi");
$p  = new Pair($bx, $by);
var_dump($p->left->value, $p->right->value);
?>
--EXPECT--
int(1)
string(2) "hi"
