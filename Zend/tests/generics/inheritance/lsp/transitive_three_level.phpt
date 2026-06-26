--TEST--
Parametric LSP: substitution composes across three levels of generic ancestors
--FILE--
<?php
interface L<T> {
    public function take(T $x): T;
}
interface M<T> extends L<T> {}
interface N<T> extends M<T> {}

class IntN implements N<int> {
    public function take(int $x): int { return $x + 1; }
}

echo (new IntN)->take(41), "\n";
?>
--EXPECT--
42
