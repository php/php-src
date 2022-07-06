--TEST--
Bug #73586 (php_user_filter::$stream is not set to the stream the filter is working on).
--FILE--
<?php
class append_filter extends php_user_filter {
    public $stream;
    function filter($in, $out, &$consumed, $closing) {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        if ($closing) {
            $bucket = stream_bucket_new($this->stream, "FooBar\n");
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("append", "append_filter");
$fin = fopen(__FILE__, 'rb');
stream_filter_append($fin, 'append', STREAM_FILTER_READ);
stream_copy_to_stream($fin, STDOUT);
?>
--EXPECT--
<?php
class append_filter extends php_user_filter {
    public $stream;
    function filter($in, $out, &$consumed, $closing) {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        if ($closing) {
            $bucket = stream_bucket_new($this->stream, "FooBar\n");
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("append", "append_filter");
$fin = fopen(__FILE__, 'rb');
stream_filter_append($fin, 'append', STREAM_FILTER_READ);
stream_copy_to_stream($fin, STDOUT);
?>
FooBar
