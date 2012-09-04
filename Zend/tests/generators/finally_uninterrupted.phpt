--TEST--
Use of finally in generator without interrupt
--FILE--
<?php

function gen() {
    try {
        throw new Exception;
    } finally {
        echo "finally run\n";
    }

    yield; // force generator
}

$gen = gen();
$gen->rewind(); // force run

?>
--EXPECTF--
finally run

Fatal error: Uncaught exception 'Exception' in %s:%d
Stack trace:
#0 [internal function]: gen()
#1 %s(%d): Generator->rewind()
#2 {main}
  thrown in %s on line %d
