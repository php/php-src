--TEST--
Phar: Phar::loadPhar with phar.extract_list
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip");?>
--INI--
phar.readonly=1
--FILE--
<?php

ini_set('phar.extract_list', 'myphar.phar=' . dirname(__FILE__));

echo Phar::loadPhar('myphar.phar');
?>
--EXPECTF--
%stests