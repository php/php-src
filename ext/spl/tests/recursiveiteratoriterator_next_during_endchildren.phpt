--TEST--
RecursiveIteratorIterator: next() re-entered from endChildren() must not use-after-free
--FILE--
<?php
class RIt implements RecursiveIterator {
    public $data;
    public $pos = 0;
    function __construct(array $d) { $this->data = $d; }
    function current(): mixed { return $this->data[$this->pos] ?? null; }
    function key(): mixed { return $this->pos; }
    function next(): void { $this->pos++; }
    function rewind(): void { $this->pos = 0; }
    function valid(): bool { return $this->pos < count($this->data); }
    function hasChildren(): bool { return is_array($this->current()); }
    function getChildren(): RecursiveIterator { return new RIt($this->current()); }
}

class ReenterRII extends RecursiveIteratorIterator {
    public static int $fired = 0;
    function endChildren(): void {
        if (self::$fired++ === 0) {
            $this->next();
        }
    }
}

$tree = [[[1, 2], 3], 4];
$it = new ReenterRII(new RIt($tree), RecursiveIteratorIterator::SELF_FIRST);
$seen = [];
foreach ($it as $v) {
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
array array 1 2 4
done
