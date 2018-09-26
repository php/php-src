--TEST--
Invalid key returned by userspace cache_key() method
--FILE--
<?php
class MyWrapper
{

public function stream_cache_key($uri)
{
return 'invalid_key';
}

}

//-----------

stream_wrapper_register('mystream','MyWrapper');

var_dump(file_cache_key('mystream://random/path'));

?>
--EXPECTF--
Fatal error: file_cache_key(): cache_key: Key (invalid_key) should start with same prefix as URL (mystream://random/path) in %s on line %d

