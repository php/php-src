--TEST--
proper string length on stream_get_contents()
--FILE--
<?php
$f = fopen('php://memory', 'rw');
fwrite($f, str_repeat('X', 1000));
fseek($f, 0);
var_dump(strlen(stream_get_contents($f, 1024)));
?>
--EXPECT--
int(1000)
