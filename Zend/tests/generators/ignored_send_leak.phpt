--TEST--
Ignoring a sent value shouldn't leak memory
--FILE--
<?php

function gen()
{
    yield;
}

$gen = gen();
$gen->send(null);

echo "DONE";

?>
--EXPECT--
DONE
