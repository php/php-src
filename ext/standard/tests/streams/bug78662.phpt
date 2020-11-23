--TEST--
Bug #78662 (stream_write bad error detection)
--FILE--
<?php
class FailedStream {
    function stream_open($path, $mode, $options, &$opened_path)
    {
        return true;
    }
    function stream_read($count)
    {
        return false;
    }
    function stream_write($data)
    {
        return false;
    }
    function stream_stat()
    {
        return [];
    }
}
stream_wrapper_register('fails', 'FailedStream');
$f=fopen('fails://foo', 'a+');
var_dump(fwrite($f, "bar"));
var_dump(fread($f, 100));
?>
Done
--EXPECT--
bool(false)
bool(false)
Done
