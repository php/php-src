--TEST--
Bug GH-10092 (Internal stream casting should not emit lost bytes warning twice)
--FILE--
<?php
$handle = fopen('http://example.com', 'r');
var_dump(stream_isatty($handle));
?>
--EXPECT--
bool(false)
