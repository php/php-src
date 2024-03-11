--TEST--
Bug GH-9259 001 (Setting opcache.interned_strings_buffer to a very high value leads to corruption of shm)
--EXTENSIONS--
opcache
--INI--
opcache.interned_strings_buffer=131072
opcache.log_verbosity_level=2
opcache.enable_cli=1
--FILE--
<?php

echo 'OK';

?>
--EXPECTF--
%sWarning opcache.interned_strings_buffer must be less than or equal to 32767, 131072 given%s

OK
