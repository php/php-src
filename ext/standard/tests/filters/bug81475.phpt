--TEST--
Bug #81475 (stream_isatty emits warning with attached stream wrapper)
--FILE--
<?php
$stdout = fopen('php://stdout', 'wb');
stream_filter_append($stdout, 'string.toupper');
var_dump(stream_isatty($stdout));
?>
--EXPECT--
bool(false)
