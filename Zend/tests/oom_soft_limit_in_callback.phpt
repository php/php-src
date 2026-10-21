--TEST--
OOM: Soft limit exceeded in library callback
--EXTENSIONS--
zend_test
--INI--
memory_limit=10M
--FILE--
<?php

function f() {
    zend_test_emalloc(10*1024*1024);
}

function shutdown_function() {
    echo "Executed\n";
}

register_shutdown_function('shutdown_function');

zend_test_callback(function () {
    f();
});

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted %S(tried to allocate %d bytes) in %s on line 4
Executed
