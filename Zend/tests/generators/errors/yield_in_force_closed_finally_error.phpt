--TEST--
yield cannot be used in a finally block when the generator is force-closed
--FILE--
<?php

function gen() {
    try {
        echo "before yield\n";
        yield;
        echo "after yield\n";
    } finally {
        echo "before yield in finally\n";
        yield;
        echo "after yield in finally\n";
    }

    echo "after finally\n";
}

$gen = gen();
$gen->rewind();
unset($gen);

?>
--EXPECTF--
before yield
before yield in finally

Fatal error: Cannot yield from finally in a force-closed generator in %s on line %d
