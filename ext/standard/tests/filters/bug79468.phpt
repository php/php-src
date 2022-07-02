--TEST--
Bug #79468	SIGSEGV when closing stream handle with a stream filter appended
--SKIPIF--
<?php
$filters = stream_get_filters();
if(! in_array( "string.rot13", $filters )) die( "skip rot13 filter not available." );
?>
--FILE--
<?php
$fp = fopen('php://temp', 'rb');
$rot13_filter = stream_filter_append($fp, "string.rot13", STREAM_FILTER_WRITE);
fwrite($fp, "This is ");
fclose($fp);
try {
    stream_filter_remove($rot13_filter);
} catch (\Throwable $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(79) "stream_filter_remove(): supplied resource is not a valid stream filter resource"
