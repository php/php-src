--TEST--
Bug #61115: Stream related segfault on fatal error in php_stream_context_del_link - variation 1
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php

$fileResourceTemp = fopen('php://temp', 'wr');
stream_context_get_options($fileResourceTemp);
ftruncate($fileResourceTemp, ((2 ** (PHP_SYS_SIZE * 8-2) - 1) << 1) + 1);
?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
