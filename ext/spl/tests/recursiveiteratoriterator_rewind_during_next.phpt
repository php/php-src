--TEST--
RecursiveIteratorIterator: rewind() re-entered from an inner next() must not use-after-free
--FILE--
<?php
class Reenter implements RecursiveIterator {
    public $data; public $pos = 0; public $rii; public $depth;
    public static bool $fired = false;
    function __construct(array $d, $depth = 0) { $this->data = $d; $this->depth = $depth; }
    function current(): mixed { return $this->data[$this->pos] ?? null; }
    function key(): mixed { return $this->pos; }
    function next(): void {
        $this->pos++;
        if ($this->rii && $this->depth === 1 && $this->pos === 1 && !self::$fired) {
            self::$fired = true;
            $this->rii->rewind();
        }
    }
    function rewind(): void { $this->pos = 0; }
    function valid(): bool { return $this->pos < count($this->data); }
    function hasChildren(): bool { return is_array($this->current()); }
    function getChildren(): RecursiveIterator {
        $c = new Reenter($this->current(), $this->depth + 1);
        $c->rii = $this->rii;
        return $c;
    }
}
$root = new Reenter([[10, 11], [20, 21]]);
$rii = new RecursiveIteratorIterator($root, RecursiveIteratorIterator::SELF_FIRST);
$root->rii = $rii;
$seen = [];
foreach ($rii as $v) {
    if (is_array($v)) { $v = '[' . implode(',', $v) . ']'; }
    $seen[] = $v;
    if (count($seen) > 20) { $seen[] = '...'; break; }
}
echo implode(' ', $seen), "\n";
echo "done\n";
?>
--EXPECT--
[10,11] 10 [10,11] 10 11 [20,21] 20 21
done
