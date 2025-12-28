--TEST--
Timeout within for loop
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows' && version_compare(PHP_VERSION, '8.4', '<')) {
    die("xfail fails on Windows Server 2022 and newer.");
}
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
