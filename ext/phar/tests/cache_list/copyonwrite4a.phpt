--TEST--
Phar: copy-on-write test 4a [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite4.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
var_dump(file_exists('phar://' . dirname(__FILE__) . '/files/write4.phar/testit.txt'));
Phar::mount('phar://' . dirname(__FILE__) . '/files/write4.phar/testit.txt', 'phar://' . dirname(__FILE__) . '/files/write4.phar/tobemounted');
var_dump(file_exists('phar://' . dirname(__FILE__) . '/files/write4.phar/testit.txt'), file_get_contents('phar://' . dirname(__FILE__) . '/files/write4.phar/testit.txt'));
?>
===DONE===
--EXPECT--
bool(false)
bool(true)
string(2) "hi"
===DONE===