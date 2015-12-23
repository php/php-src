--TEST--
Plain file is cacheable
--FILE--
<?php
function check_cache_key($path)
{
$key=file_cache_key($path);
var_dump($key);
}
//-----------

check_cache_key('/Foo/Bar/Moo.php');
check_cache_key('file:///Foo/Bar/Moo.php');
?>
--EXPECTF--
string(16) "/Foo/Bar/Moo.php"
string(23) "file:///Foo/Bar/Moo.php"
