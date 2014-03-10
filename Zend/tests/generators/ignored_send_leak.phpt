--TEST--
Ignoring a sent value shouldn't leak memory
--FILE--
<?php

function gen() {
    yield;
}

$gen = gen();
$gen->send(NULL);

echo "DONE";

?>
--EXPECT--
DONE
