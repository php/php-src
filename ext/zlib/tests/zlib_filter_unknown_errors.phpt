--TEST--
zlib.deflate filter param errors
--EXTENSIONS--
zlib
--FILE--
<?php
$fp = fopen('php://stdout', 'w');

var_dump(stream_filter_append($fp, 'zlib.unknown', STREAM_FILTER_WRITE));

fclose($fp);

?>
--EXPECTF--
Warning: stream_filter_append(): Unable to create or locate filter "zlib.unknown" in %s on line %d
bool(false)
