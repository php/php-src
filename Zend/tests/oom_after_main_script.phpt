--TEST--
OOM: shutdown functions run after an OOM raised at the end of the main script
--INI--
memory_limit=8M
opcache.optimization_level=0
--FILE--
<?php

register_shutdown_function(function () {
    echo "Shutdown function called\n";
});

function test() {
    $a = str_repeat('a', 3 * 1024 * 1024);
    $b = $a . $a;
    echo "Done\n";
}

test();

?>
--EXPECTF--
Done

Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
Shutdown function called
