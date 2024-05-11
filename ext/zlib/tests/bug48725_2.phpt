--TEST--
Bug #48725 (Support for flushing in zlib stream)
--EXTENSIONS--
zlib
--FILE--
<?php
$stream = fopen('data://text/plain;base64,' . base64_encode('Foo bar baz'), 
'r');
stream_filter_append($stream, 'zlib.deflate', STREAM_FILTER_READ);
print gzinflate(stream_get_contents($stream));
?>
--EXPECT--
Foo bar baz
