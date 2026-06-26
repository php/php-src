--TEST--
Recursive bounds: invariant class params with mutual bounds are accepted
--FILE--
<?php
class Box<T> {}
class Pair<T: Box<U>, U: Box<T>> {
    public function __construct(public Box $left, public Box $right) {}
}
echo "ok\n";
?>
--EXPECT--
ok
