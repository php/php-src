--TEST--
Parametric LSP: child overrides param and return with the bound argument
--FILE--
<?php
interface Bag<T> {
    public function put(T $value): void;
    public function get(): T;
}

class IntBag implements Bag<int> {
    private int $v = 0;
    public function put(int $value): void { $this->v = $value; }
    public function get(): int { return $this->v; }
}

$b = new IntBag;
$b->put(42);
echo $b->get(), "\n";

try {
    /** @phpstan-ignore-next-line */
    $b->put("no");
} catch (TypeError) {
    echo "TypeError\n";
}
?>
--EXPECT--
42
TypeError
