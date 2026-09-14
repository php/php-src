--TEST--
Flushing a read filter on a partially-read stream must not duplicate buffered bytes
--FILE--
<?php
class FlushEmitFilter extends php_user_filter
{
    public function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        if ($closing) {
            stream_bucket_append($out, stream_bucket_new($this->stream, "<FLUSH>"));
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register("flushemit", "FlushEmitFilter");

$fp = fopen("php://memory", "r+");
fwrite($fp, "ABCDEFGHIJKLMNOP");
rewind($fp);

$filter = stream_filter_append($fp, "flushemit", STREAM_FILTER_READ);
var_dump(fread($fp, 5));
stream_filter_remove($filter);
var_dump(fread($fp, 100));
?>
--EXPECT--
string(5) "ABCDE"
string(18) "FGHIJKLMNOP<FLUSH>"
