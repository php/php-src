--TEST--
Parametric LSP: variadic parameter substituted with the bound argument
--FILE--
<?php
interface Acceptor<T> {
    public function acceptAll(T ...$items): int;
}

class IntAcceptor implements Acceptor<int> {
    public function acceptAll(int ...$items): int { return array_sum($items); }
}

echo (new IntAcceptor)->acceptAll(1, 2, 3, 4), "\n";

try {
    /** @phpstan-ignore-next-line */
    (new IntAcceptor)->acceptAll(1, "no");
} catch (TypeError) {
    echo "TypeError\n";
}
?>
--EXPECT--
10
TypeError
