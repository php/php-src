--TEST--
Bug GH-9259 002 (Setting opcache.interned_strings_buffer to a very high value leads to corruption of shm)
--EXTENSIONS--
opcache
--INI--
opcache.interned_strings_buffer=-1
opcache.log_verbosity_level=2
opcache.enable_cli=1
--FILE--
<?php

echo 'OK';

?>
--EXPECTF--
%sWarning opcache.interned_strings_buffer must be greater than or equal to 0, -1 given%s

OK
