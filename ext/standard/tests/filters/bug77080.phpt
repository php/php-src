--TEST--
Bug #77080 (Deflate not working)
--EXTENSIONS--
zlib
--FILE--
<?php
$string = str_repeat("0123456789", 100);
$stream = fopen('data://text/plain,' . $string,'r');
stream_filter_append($stream, 'zlib.deflate', STREAM_FILTER_READ, 6);
$compressed = stream_get_contents($stream);
var_dump(gzinflate($compressed) === $string);
?>
--EXPECT--
bool(true)
