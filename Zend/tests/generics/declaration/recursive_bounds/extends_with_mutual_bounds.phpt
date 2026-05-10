--TEST--
Recursive bounds: a child class can extend a parent that uses mutual bounds
--FILE--
<?php
class Box<T> {}
class Pair<T: Box<U>, U: Box<T>> {
    public function __construct(public Box $left, public Box $right) {}
}

class StrictPair<A: Box<B>, B: Box<A>> extends Pair<A, B> {}

$p = new StrictPair::<Box<int>, Box<string>>(new Box(), new Box());
var_dump($p instanceof Pair, $p instanceof StrictPair);
?>
--EXPECT--
bool(true)
bool(true)
