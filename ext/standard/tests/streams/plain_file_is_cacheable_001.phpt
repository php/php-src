--TEST--
Plain file is cacheable
--FILE--
<?php
function check_cacheable($path)
{
$ret=file_is_cacheable($path);
echo "$path is ".($ret ? '' : 'not ')."cacheable\n";
}
//-----------

check_cacheable('/Foo/Bar/Moo.php');
check_cacheable('file:///Foo/Bar/Moo.php');
?>
--EXPECTF--
%s is cacheable
%s is cacheable
