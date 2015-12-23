--TEST--
Null cache key returned from userspace wrapper
--FILE--
<?php
class MyWrapper
{
public function stream_cache_key($uri)
{
return null;
}

}

//-----------

stream_wrapper_register('mystream','MyWrapper');

var_dump(file_cache_key('mystream://random/path'));

?>
--EXPECTF--
NULL
