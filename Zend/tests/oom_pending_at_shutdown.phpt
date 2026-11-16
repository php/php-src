--TEST--
An OOM left pending by a shutdown function is reported
--INI--
memory_limit=16M
--FILE--
<?php
register_shutdown_function(function () {
    echo "Shutdown function called\n";
    $a = str_repeat('a', 6 * 1024 * 1024);
    $GLOBALS['b'] = $a . $a;
});
?>
--EXPECTF--
Shutdown function called

Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
