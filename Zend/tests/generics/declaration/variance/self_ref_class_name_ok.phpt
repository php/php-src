--TEST--
Variance: a generic class referencing itself by name in a covariant return position
--FILE--
<?php
final readonly class Pair<+L, +R> {
    public function __construct(public L $left, public R $right) {}
    public function swap(): Pair<R, L> {
        return new Pair($this->right, $this->left);
    }
}

$p = new Pair::<int, string>(7, "hi");
$s = $p->swap();
var_dump($s->left, $s->right);
?>
--EXPECT--
string(2) "hi"
int(7)
