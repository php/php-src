--TEST--
Timeout again inside register_shutdown_function
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
if (PHP_ZTS) die("skip hard_timeout works only on no-zts builds");
?>
--INI--
hard_timeout=1
--FILE--
<?php
set_time_limit(1);
register_shutdown_function("plop");

function plop() {
    while (true);
}
plop();
?>
===DONE===
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d

Fatal error: Maximum execution time of 1+1 seconds exceeded (terminated) in %s on line %d
