--TEST--
RecursiveIteratorIterator: rewind() re-entered from hasChildren() throws
--FILE--
<?php
// A reentrant rewind() used to run spl_recursive_it_rewind_ex(), tearing down
// every active level and erealloc()ing object->iterators back to a single
// element while the outer spl_recursive_it_move_forward_ex() frame was still mid
// hasChildren(), leaving the cached sub-object and iterators[] dangling.
// hasChildren() must not mutate the iteration state, so this now throws instead.
class RIt implements RecursiveIterator {
    public $data;
    public $pos = 0;
    public $rii;
    public $depth;
    public static bool $fired = false;
    function __construct(array $d, $depth = 0) { $this->data = $d; $this->depth = $depth; }
    function current(): mixed { return $this->data[$this->pos] ?? null; }
    function key(): mixed { return $this->pos; }
    function next(): void { $this->pos++; }
    function rewind(): void { $this->pos = 0; }
    function valid(): bool { return $this->pos < count($this->data); }
    function hasChildren(): bool {
        if ($this->rii && $this->depth >= 1 && !self::$fired) {
            self::$fired = true;
            $this->rii->rewind();
        }
        return is_array($this->current());
    }
    function getChildren(): RecursiveIterator {
        $c = new RIt($this->current(), $this->depth + 1);
        $c->rii = $this->rii;
        return $c;
    }
}

$root = new RIt([[[1, 2], 3], 4]);
$rii = new RecursiveIteratorIterator($root, RecursiveIteratorIterator::SELF_FIRST);
$root->rii = $rii;
try {
    foreach ($rii as $v) {
    }
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
echo "done\n";
?>
--EXPECT--
Cannot rewind a RecursiveIteratorIterator from within hasChildren() or getChildren()
done
