--TEST--
finally is run even if a generator is closed mid-execution
--FILE--
<?php

function gen() {
    try {
        try {
            echo "before yield\n";
            yield;
            echo "after yield\n";
        } finally {
            echo "finally run\n";
        }
        echo "code after finally\n";
    } finally {
        echo "second finally run\n";
    }
    echo "code after second finally\n";
}

$gen = gen();
$gen->rewind();
unset($gen);

?>
--EXPECT--
before yield
finally run
second finally run
