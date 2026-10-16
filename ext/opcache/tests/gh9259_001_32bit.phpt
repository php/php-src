--TEST--
Bug GH-9259 001 (Setting opcache.interned_strings_buffer to a very high value leads to corruption of shm) - 32bit
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (PHP_SYS_SIZE != 4) die("skip: 32-bit platform only"); ?>
--INI--
opcache.interned_strings_buffer=131072
opcache.log_verbosity_level=2
opcache.enable_cli=1
--FILE--
<?php

echo 'OK';

?>
--EXPECTF--
%sWarning opcache.interned_strings_buffer must be less than or equal to 4095, 131072 given%s

OK
