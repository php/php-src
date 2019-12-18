--TEST--
Generator exceptions during shutdown should not be swallowed
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

?>
--EXPECTF--
before yield
before yield in finally

Fatal error: Uncaught Error: Cannot yield from finally in a force-closed generator in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
