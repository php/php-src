--TEST--
Generic interface: basic declaration
--FILE--
<?php
interface Collection<T> {
    public function add(T $item): void;
    public function get(int $index): T;
}

echo "Collection<T> interface declared\n";
?>
--EXPECT--
Collection<T> interface declared
