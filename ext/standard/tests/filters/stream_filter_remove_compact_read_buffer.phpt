--TEST--
stream_filter_remove() compacts unread data before appending flushed data
--FILE--
<?php
class ClosingSuffixFilter extends php_user_filter
{
    public function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        if ($closing) {
            stream_bucket_append($out, stream_bucket_new($this->stream, 'END'));
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register('closing-suffix', ClosingSuffixFilter::class);
$stream = fopen('php://memory', 'w+');
fwrite($stream, 'abcdef');
rewind($stream);
$filter = stream_filter_append($stream, 'closing-suffix', STREAM_FILTER_READ);
var_dump(fread($stream, 2));
var_dump(stream_filter_remove($filter));
var_dump(stream_get_contents($stream));
?>
--EXPECT--
string(2) "ab"
bool(true)
string(7) "cdefEND"
