--TEST--
Bug #43216 (stream_is_local() returns false on file://)
--FILE--
<?
var_dump(stream_is_local("file://"));
?>
--EXPECT--
bool(true)
