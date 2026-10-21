--TEST--
OOM: the message returned by error_get_last() can outlive the error state
--INI--
memory_limit=8M
opcache.optimization_level=0
--FILE--
<?php

register_shutdown_function(function () {
    $error = error_get_last();
    error_clear_last();
    echo $error['message'], "\n";
});

function test() {
    $a = [];
    while (true) {
        $a[] = str_repeat('a', 1024);
    }
}

test();

?>
--EXPECTF--
Fatal error: Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
Allowed memory size of 8388608 bytes exhausted %S(tried to allocate %d bytes)
