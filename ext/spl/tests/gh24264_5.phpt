--TEST--
GH-24264 (Unwind/GracefulExit discarded in spl_iterators.c): destroyed fiber suspended in getChildren()
--FILE--
<?php

class ReproIterator extends RecursiveArrayIterator {
    public function getChildren(): ?RecursiveArrayIterator {
        try {
            Fiber::suspend();
        } finally {
            echo "finally\n";
        }
    }
}

$fiber = new Fiber(function () {
    $it = new RecursiveIteratorIterator(
        new ReproIterator([[1], [2]]),
        RecursiveIteratorIterator::LEAVES_ONLY,
        RecursiveIteratorIterator::CATCH_GET_CHILD,
    );

    foreach ($it as $_) {}

    echo "Not reached\n";
});

$fiber->start();
unset($fiber);

echo "Done\n";
?>
--EXPECT--
finally
Done
