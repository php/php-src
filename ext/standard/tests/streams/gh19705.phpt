--TEST--
GH-19705 segmentation fault with non writable stream at stream_filter_append call.
--EXTENSIONS--
zlib
--FILE--
<?php
$fh = fopen('data://text/plain,', 'w+',);
var_dump(stream_filter_append($fh, 'zlib.deflate',STREAM_FILTER_WRITE));
?>
--EXPECTF--
resource(%d) of type (stream filter)
