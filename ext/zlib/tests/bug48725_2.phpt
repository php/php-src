--TEST--
Bug #48725 (Support for flushing in zlib stream)
--EXTENSIONS--
zlib
--FILE--
<?php
$stream = fopen('data://text/plain;base64,' . base64_encode('Foo bar baz'), 
'r');
stream_filter_append($stream, 'zlib.deflate', STREAM_FILTER_READ);
print bin2hex(stream_get_contents($stream));
?>
--EXPECT--
72cbcf57484a2c02e22a00000000ffff0300
