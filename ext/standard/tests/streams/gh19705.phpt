--TEST--
GH-19705 segmentation fault with non writable stream at stream_filter_append call.
--EXTENSIONS--
zlib
--FILE--
<?php
$fh = fopen('data://text/plain,', 'w+',);
$v_249671 = stream_filter_append($fh, 'zlib.deflate',2,);
?>
--EXPECTF--

Notice: PHP Request Shutdown: Stream is not writable in Unknown on line %d
