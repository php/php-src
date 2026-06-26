--TEST--
Recursive bounds: turbofish argument satisfying the head class is accepted
--FILE--
<?php
class Box<T> {}
class Pair<T: Box<U>, U: Box<T>> {}

$p = new Pair::<Box<int>, Box<string>>();
var_dump($p instanceof Pair);
?>
--EXPECT--
bool(true)
