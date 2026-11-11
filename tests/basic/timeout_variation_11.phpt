--TEST--
Timeout within shutdown function, user function variation
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--INI--
max_execution_time=1
--FILE--
<?php

register_shutdown_function(function () {
    while (true) {}
});
register_shutdown_function(function () {
    echo "Not executed\n";
    while (true) {}
});

?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %stimeout_variation_11.php on line %d
