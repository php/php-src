--TEST--
RecursiveIteratorIterator: next() re-entered from a sub-iterator valid() must not use-after-free
--FILE--
<?php
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
    function valid(): bool {
        if ($this->rii && $this->depth === 2 && $this->pos === count($this->data) && !self::$fired) {
            self::$fired = true;
            $this->rii->next();
        }
        return $this->pos < count($this->data);
    }
    function hasChildren(): bool { return is_array($this->current()); }
    function getChildren(): RecursiveIterator {
        $c = new RIt($this->current(), $this->depth + 1);
        $c->rii = $this->rii;
        return $c;
    }
}

$root = new RIt([[[1, 2], 3], 4]);
$rii = new RecursiveIteratorIterator($root, RecursiveIteratorIterator::SELF_FIRST);
$root->rii = $rii;
$seen = [];
foreach ($rii as $v) {
    $seen[] = is_array($v) ? 'array' : $v;
    if (count($seen) > 20) {
        $seen[] = '...';
        break;
    }
}
echo implode(' ', $seen), "\n";
echo "done\n";
?>
--EXPECT--
array array 1 2 3 4
done
