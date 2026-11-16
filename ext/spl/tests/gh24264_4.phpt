--TEST--
GH-24264 (Unwind/GracefulExit discarded in spl_iterators.c): RecursiveCachingIterator getChildren()
--FILE--
<?php

class ReproIterator extends RecursiveArrayIterator {
    public function getChildren(): ?RecursiveArrayIterator {
        echo "exit\n";
        exit(1);
    }
}

$it = new RecursiveCachingIterator(
    new ReproIterator([[1]]),
    CachingIterator::CATCH_GET_CHILD,
);

foreach ($it as $_) {}

echo "Not reached\n";
?>
--EXPECT--
exit
