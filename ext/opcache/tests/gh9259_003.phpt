--TEST--
Bug GH-9259 003 (Setting opcache.interned_strings_buffer to a very high value leads to corruption of shm)
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) die('xfail Leaks memory with ASAN');
?>
--INI--
opcache.interned_strings_buffer=500
opcache.enable_cli=1
--FILE--
<?php

echo 'OK';

?>
--EXPECTF--
%sFatal Error Insufficient shared memory for interned strings buffer! (tried to allocate %d bytes)
