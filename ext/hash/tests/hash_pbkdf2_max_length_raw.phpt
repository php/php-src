--TEST--
Hash: hash_pbkdf2() function : raw output length of PHP_INT_MAX
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) die("skip this test is not for 32bit platforms");
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--INI--
memory_limit=128M
--FILE--
<?php

hash_pbkdf2('md5', 'password', 'salt', 1, PHP_INT_MAX, true);

?>
--EXPECTF--
Fatal error: Allowed memory size of 134217728 bytes exhausted%s(tried to allocate 9223372036854775808 bytes) in %s on line %d
