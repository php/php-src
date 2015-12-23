--TEST--
HTTP URL is not cacheable
--FILE--
<?php
function check_cache_key($path)
{
$key=file_cache_key($path);
var_dump($key);
}
//-----------


check_cache_key('http://acme.com/Foo.php');
check_cache_key('https://acme.com/Foo.php');
?>
--EXPECTF--
NULL
NULL
