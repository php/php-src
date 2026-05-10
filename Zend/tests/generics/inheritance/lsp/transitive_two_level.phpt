--TEST--
Parametric LSP: substitution composes through a generic intermediate
--FILE--
<?php
interface Consumer<T> {
    public function consume(T $value): void;
}

interface Service<T> extends Consumer<T> {}

class NullService implements Service<null> {
    public function consume(null $value): void {
        echo "consumed null\n";
    }
}

(new NullService)->consume(null);
?>
--EXPECT--
consumed null
