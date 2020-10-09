--TEST--
Timeout within foreach loop
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

set_time_limit(1);

foreach (new InfiniteIterator(new ArrayIterator([1])) as $i) {
}

?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
