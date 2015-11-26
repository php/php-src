--TEST--
Timeout again inside register_shutdown_function
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
set_time_limit(1);
register_shutdown_function("plop");

function plop() {
    $ts = time();
    while(true) {
        if ((time()-$ts) > 2) {
            echo "Failed!";
            break;
        }
    }
}
plop();
?>
===DONE===
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d

Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
