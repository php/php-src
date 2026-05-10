--TEST--
Parametric LSP: multi-parameter generic interface substitutes both parameters
--FILE--
<?php
interface Map<K, V> {
    public function set(K $key, V $value): void;
    public function lookup(K $key): V;
}

class StrIntMap implements Map<string, int> {
    private array $items = [];
    public function set(string $key, int $value): void { $this->items[$key] = $value; }
    public function lookup(string $key): int { return $this->items[$key] ?? -1; }
}

$m = new StrIntMap;
$m->set("a", 1);
echo $m->lookup("a"), "\n";
echo $m->lookup("missing"), "\n";
?>
--EXPECT--
1
-1
