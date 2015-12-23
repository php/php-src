--TEST--
Unimplemented cache_key() method in userspace wrapper
--FILE--
<?php
class MyWrapper
{

}

//-----------

stream_wrapper_register('mystream','MyWrapper');

var_dump(file_cache_key('mystream://random/path'));

?>
--EXPECTF--
NULL
