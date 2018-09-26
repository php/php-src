--TEST--
Valid cache key returned from userspace wrapper
--FILE--
<?php
class MyWrapper
{
public function stream_cache_key($uri)
{
return $uri.':key';
}

}

//-----------

stream_wrapper_register('mystream','MyWrapper');

var_dump(file_cache_key('mystream://random/path'));

?>
--EXPECTF--
string(26) "mystream://random/path:key"
