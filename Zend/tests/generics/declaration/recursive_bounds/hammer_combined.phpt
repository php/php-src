--TEST--
Recursive bounds: combined exercise — class, interface, generic methods, multi-cycle
--FILE--
<?php
interface Box<X> {}
class IntBox implements Box<int> {}
class StrBox implements Box<string> {}

interface Pair<T: Box<U>, U: Box<T>> {
    public function left(): Box;
    public function right(): Box;
}

class IntStrPair implements Pair<IntBox, StrBox> {
    public function __construct(private Box $l, private Box $r) {}
    public function left(): Box { return $this->l; }
    public function right(): Box { return $this->r; }
}

class Container<O> {
    public function swap<A: Box<B>, B: Box<A>>(Pair $p): array {
        return [$p->right(), $p->left()];
    }
}

$p = new IntStrPair(new IntBox(), new StrBox());
$c = new Container::<int>();
[$first, $second] = $c->swap($p);

var_dump($first instanceof StrBox, $second instanceof IntBox);
?>
--EXPECT--
bool(true)
bool(true)
