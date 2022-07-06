--TEST--
Bug #62474 (com_event_sink crashes on certain arguments)
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet extension not available');
?>
--FILE--
<?php
var_dump(com_event_sink(new variant, function() {}, array()));
var_dump(com_event_sink(new variant, new variant, 'a'));
?>
--EXPECT--
bool(false)
bool(false)
