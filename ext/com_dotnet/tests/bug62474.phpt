--TEST--
Bug #62474 (com_event_sink crashes on certain arguments)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
var_dump(com_event_sink(new variant, function() {}, array()));
var_dump(com_event_sink(new variant, new variant, 'a'));
?>
--EXPECT--
bool(false)
bool(false)
