--TEST--
Phar: test nested linked files
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
echo file_get_contents('phar://' . __DIR__ . '/files/links.phar.tar/link2');
echo file_get_contents('phar://' . __DIR__ . '/files/links.phar.tar/link1');
echo file_get_contents('phar://' . __DIR__ . '/files/links.phar.tar/testit.txt');

$a = fopen('phar://' . __DIR__ . '/files/links.phar.tar/link2', 'r');
fseek($a, 3);
echo fread($a, 10);
?>
--EXPECT--
hi there

hi there

hi there

there

