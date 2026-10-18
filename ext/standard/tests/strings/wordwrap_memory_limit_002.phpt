--TEST--
wordwrap() OOM - single character break
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

$str = str_repeat('a', 6*1024*1024);
var_dump(wordwrap($str, 80, "\n", false));

?>
--EXPECTF--
Fatal error: Allowed memory size of 10485760 bytes exhausted%s(tried to allocate %d bytes) in %s on line 4
