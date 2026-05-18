--TEST--
Parametric LSP: a default type parameter still allows the child's signature to be erased-compatible
--FILE--
<?php
interface Holder<T = int> {
    public function set(T $value): void;
}

class IntHolder implements Holder {
    public function set(int $value): void { echo "set $value\n"; }
}

(new IntHolder)->set(5);
?>
--EXPECT--
set 5
