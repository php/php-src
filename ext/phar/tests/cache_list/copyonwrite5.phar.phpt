--TEST--
Phar: copy-on-write test 5 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite5.phar.php
phar.readonly=0
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE_EXTERNAL--
files/write5.phar
--CLEAN--
<?php
unlink(__DIR__ . '/copyonwrite5/file1');
unlink(__DIR__ . '/copyonwrite5/file2');
rmdir(__DIR__ . '/copyonwrite5');
?>
--EXPECTF--
array(2) {
  ["file1"]=>
  string(%d) "%sfile1"
  ["file2"]=>
  string(%d) "%sfile2"
}
phar://%scopyonwrite5.phar.php%cfile1 file1
phar://%scopyonwrite5.phar.php%cfile2 file2
phar://%scopyonwrite5.phar.php%chi hi
ok
