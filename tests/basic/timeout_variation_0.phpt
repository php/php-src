--TEST--
Timeout within while loop
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
$tracing = extension_loaded("Zend OPcache")
    && ($conf = opcache_get_configuration()["directives"])
    && array_key_exists("opcache.jit", $conf)
    &&  $conf["opcache.jit"] === "tracing";
if (PHP_OS_FAMILY === "Windows" && PHP_INT_SIZE == 8 && $tracing && getenv('SKIP_ASAN')) {
    $url = "https://github.com/php/php-src/issues/15709";
    die("xfail Test fails on Windows x64 (VS17) and tracing JIT with ASan; see $url");
}
?>
--FILE--
<?php

set_time_limit(1);

$x = true;
$y = 0;
while ($x) {
    $y++;
}

?>
never reached here
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
