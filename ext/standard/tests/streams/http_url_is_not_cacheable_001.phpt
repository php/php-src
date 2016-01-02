--TEST--
HTTP URL is not cacheable
--FILE--
<?php
function check_cacheable($path)
{
$ret=file_is_cacheable($path);
echo "$path is ".($ret ? '' : 'not ')."cacheable\n";
}
//-----------

check_cacheable('http://acme.com/Foo.php');
check_cacheable('https://acme.com/Foo.php');
?>
--EXPECTF--
%s is not cacheable
%s is not cacheable
