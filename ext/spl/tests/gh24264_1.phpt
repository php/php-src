--TEST--
GH-24264 (Unwind/GracefulExit discarded in spl_iterators.c): RecursiveIteratorIterator hasChildren()
--FILE--
<?php

class ReproIterator extends RecursiveArrayIterator {
    public function hasChildren(): bool {
        echo "exit\n";
        exit(1);
    }
}

$it = new RecursiveIteratorIterator(
    new ReproIterator([1]),
    RecursiveIteratorIterator::LEAVES_ONLY,
    RecursiveIteratorIterator::CATCH_GET_CHILD,
);

foreach ($it as $_) {}

echo "Not reached\n";
?>
--EXPECT--
exit
