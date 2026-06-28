--TEST--
RecursiveIteratorIterator: next() re-entered from getChildren() throws
--FILE--
<?php
// getChildren() must not advance the outer iterator either: a reentrant next()
// runs spl_recursive_it_move_forward_ex() while the outer frame is mid
// getChildren(), so it is guarded the same way as the reentrant rewind().
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
    function hasChildren(): bool { return is_array($this->current()); }
    function getChildren(): RecursiveIterator {
        if ($this->rii && $this->depth >= 1 && !self::$fired) {
            self::$fired = true;
            $this->rii->next();
        }
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
Cannot advance a RecursiveIteratorIterator from within hasChildren() or getChildren()
done
