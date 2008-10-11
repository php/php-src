--TEST--
Phar: test nested linked files
--SKIPIF--
<?php
if (!extension_loaded("phar")) die("skip");
?>
--FILE--
<?php
echo file_get_contents('phar://' . dirname(__FILE__) . '/files/links.phar.tar/link2');
echo file_get_contents('phar://' . dirname(__FILE__) . '/files/links.phar.tar/link1');
echo file_get_contents('phar://' . dirname(__FILE__) . '/files/links.phar.tar/testit.txt');
?>
===DONE===
--EXPECT--
hi there

hi there

hi there

===DONE===