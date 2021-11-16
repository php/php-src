--TEST--
Phar: copy-on-write test 4a [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite4.phar.php
phar.readonly=0
--EXTENSIONS--
phar
--FILE--
<?php
var_dump(file_exists('phar://' . __DIR__ . '/files/write4.phar/testit.txt'));
Phar::mount('phar://' . __DIR__ . '/files/write4.phar/testit.txt', 'phar://' . __DIR__ . '/files/write4.phar/tobemounted');
var_dump(file_exists('phar://' . __DIR__ . '/files/write4.phar/testit.txt'), file_get_contents('phar://' . __DIR__ . '/files/write4.phar/testit.txt'));
?>
--EXPECT--
bool(false)
bool(true)
string(2) "hi"
