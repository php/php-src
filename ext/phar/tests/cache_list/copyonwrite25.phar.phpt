--TEST--
Phar: copy-on-write test 25 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite25.phar.php
phar.readonly=0
--EXTENSIONS--
phar
zlib
--FILE_EXTERNAL--
files/write25.phar
--EXPECT--
bool(false)
bool(true)
<?php __HALT_COMPILER();
ok
