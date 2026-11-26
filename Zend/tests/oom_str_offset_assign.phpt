--TEST--
OOM: string offset assignment beyond the memory limit
--INI--
memory_limit=100M
--FILE--
<?php

function shutdown_function() {
    echo "Shutdown function executed\n";
}

register_shutdown_function('shutdown_function');

$s = 'a';
$s[300<<20] = 'b';

?>
--EXPECTF--
Fatal error: Allowed memory size of 104857600 bytes exhausted %S(tried to allocate %d bytes) in %s on line 10
Shutdown function executed
