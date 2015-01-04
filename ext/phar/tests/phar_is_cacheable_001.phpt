--TEST--
Phar URI is cacheable
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
function check_cacheable($path)
{
$ret=file_is_cacheable($path);
echo "$path is ".($ret ? '' : 'not ')."cacheable\n";
}
//-----------

check_cacheable('phar:///Foo/Bar/Moo.phar.php');
?>
--EXPECTF--
%s is cacheable
