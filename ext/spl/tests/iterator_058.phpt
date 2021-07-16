--TEST--
SPL: Iterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myIterator implements Iterator {

    function current(): mixed {}
    function next(): void {}
    function key(): mixed {}
    function valid(): bool {}
    function rewind(): void {}

}
try {
    $it = new myIterator();
} catch (InvalidArgumentException $e) {
    echo 'InvalidArgumentException thrown';
}
echo 'no Exception thrown';
?>
--EXPECT--
no Exception thrown
