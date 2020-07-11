--TEST--
SPL: Iterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myIterator implements Iterator {

    function current() {}
    function next() {}
    function key() {}
    function valid() {}
    function rewind() {}

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
