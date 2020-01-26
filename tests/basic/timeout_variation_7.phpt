--TEST--
Timeout within for loop
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

set_time_limit(1);

$y = 0;
for ($i = 0; $i < INF; $i++) {
    $y++;
}

?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
