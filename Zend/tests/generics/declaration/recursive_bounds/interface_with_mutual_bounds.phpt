--TEST--
Recursive bounds: an interface can declare mutual bounds
--FILE--
<?php
interface Box<T> {}
interface PairLike<T: Box<U>, U: Box<T>> {
    public function left(): Box;
    public function right(): Box;
}

class IntBox implements Box<int> {}
class StrBox implements Box<string> {}

class Pair implements PairLike<IntBox, StrBox> {
    public function __construct(private Box $l, private Box $r) {}
    public function left(): Box { return $this->l; }
    public function right(): Box { return $this->r; }
}

$p = new Pair(new IntBox(), new StrBox());
var_dump($p->left() instanceof IntBox, $p->right() instanceof StrBox);
?>
--EXPECT--
bool(true)
bool(true)
