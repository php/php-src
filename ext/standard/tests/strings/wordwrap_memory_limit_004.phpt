--TEST--
wordwrap() OOM - extend
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--INI--
memory_limit=10M
--FILE--
<?php

// Existing break chars in str cause the initialize allocation to be too short.
// OOM while extending string.
$str = str_repeat("\r\n", 2*1024*1024);
wordwrap($str, strlen($str), "\r\n", false);

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted%s(tried to allocate %d bytes) in %s on line 6
