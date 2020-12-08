--TEST--
Timeout in loop
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php

ini_set("max_execution_time", 1);
ini_set("max_execution_wall_time", 1);

for ($i = 0; $i < 3; $i++) {
    sleep(1);
}

echo "Never reached";

?>
--EXPECTF--
Fatal error: Maximum execution wall-time of 1 second exceeded in %s on line %d
