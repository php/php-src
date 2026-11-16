--TEST--
OOM: Soft limit exceeded in FCALL
--INI--
memory_limit=10M
--FILE--
<?php

function f() {
    $a = [];
    while (true) {
        array_push($a, str_repeat('a', 1024*1024));
    }
}

f();

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes %s (tried to allocate %d bytes) in %s on line %d
