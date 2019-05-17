--TEST--
try { return } finally { return } in generator
--FILE--
<?php

function gen() {
    try {
        try {
            echo "before return\n";
            return;
            echo "after return\n";
        } finally {
            echo "before return in inner finally\n";
            return;
            echo "after return in inner finally\n";
        }
    } finally {
        echo "outer finally run\n";
    }

    echo "code after finally\n";

    yield; // force generator
}

$gen = gen();
$gen->rewind(); // force run

?>
--EXPECT--
before return
before return in inner finally
outer finally run
