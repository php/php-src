--TEST--
The original argv is restored before atexit() handlers run
--EXTENSIONS--
zend_test
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Darwin') {
    die('skip macOS only');
}
?>
--ENV--
ZEND_DONT_UNLOAD_MODULES=1
ZEND_TEST_ARGV_ATEXIT=1
--FILE--
<?php

cli_set_process_title('php-test');

// Keep the extension loaded so that the handler runs at process exit, after
// cleanup_ps_args(), and not when the module is unloaded.
zend_test_register_argv_atexit();

?>
--EXPECT--
argv_changed=1
argv_valid=1
