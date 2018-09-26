--TEST--
Phar URI is cacheable and key is URI
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
function check_cache_key($path)
{
$key=file_cache_key($path);
var_dump($key);
}
//-----------

check_cache_key('phar:///Foo/Bar/Moo.phar.php');
?>
==DONE==
--EXPECTF--
string(28) "phar:///Foo/Bar/Moo.phar.php"
==DONE==
