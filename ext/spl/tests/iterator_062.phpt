--TEST--
SPL: RecursiveIteratorIterator::__construct(void)
--CREDITS--
Sebastian Schürmann
--FILE--
<?php
class myRecursiveIteratorIterator extends RecursiveIteratorIterator {

}

try {
    $it = new myRecursiveIteratorIterator();
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
RecursiveIteratorIterator::__construct() expects at least 1 argument, 0 given
