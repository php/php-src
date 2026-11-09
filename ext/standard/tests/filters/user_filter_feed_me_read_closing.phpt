--TEST--
Input kept back by a read filter with PSFS_FEED_ME is re-presented on the closing call
--FILE--
<?php
class defer extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        $data = '';
        while ($b = stream_bucket_make_writeable($in)) { $data .= $b->data; }
        if (!$closing) {
            stream_bucket_prepend($in, stream_bucket_new($this->stream, $data));
            return PSFS_FEED_ME;
        }
        stream_bucket_append($out, stream_bucket_new($this->stream, "[" . $data . "]"));
        return PSFS_PASS_ON;
    }
}
stream_filter_register("defer", "defer");
$f = __DIR__ . "/user_filter_feed_me_read_closing.bin";
file_put_contents($f, "abcdefghij");
$fp = fopen($f, 'r');
stream_set_chunk_size($fp, 4);
stream_filter_append($fp, "defer", STREAM_FILTER_READ);
var_dump(stream_get_contents($fp));
fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_feed_me_read_closing.bin");
?>
--EXPECT--
string(12) "[abcdefghij]"
