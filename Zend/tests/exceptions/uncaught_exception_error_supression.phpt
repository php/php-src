--TEST--
Error suppression should have no impact on uncaught exceptions
--FILE--
<?php

function abc() {
    throw new Error('Example Exception');
}

@abc();

?>
--EXPECTF--
Fatal error: Uncaught Error: Example Exception in %s:%d
Stack trace:
#0 %s(%d): abc()
#1 {main}
  thrown in %s on line %d
