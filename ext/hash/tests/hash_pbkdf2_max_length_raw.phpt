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
Hard memory limit of 268435456 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d

Termsig=9
