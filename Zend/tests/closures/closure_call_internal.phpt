--TEST--
Closure::call() on internal method
--FILE--
<?php

var_dump(Closure::fromCallable([new DateTime(), 'getTimestamp'])->call(new DateTime('@123')));

?>
--EXPECT--
int(123)
