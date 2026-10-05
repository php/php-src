--TEST--
Input left on the brigade by a read filter returning PSFS_PASS_ON is discarded with a warning
--FILE--
<?php
class lines extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        $data = '';
        while ($b = stream_bucket_make_writeable($in)) { $data .= $b->data; $consumed += $b->datalen; }
        $p = strrpos($data, "\n");
        if ($p === false) {
            return PSFS_FEED_ME;
        }
        stream_bucket_append($out, stream_bucket_new($this->stream, substr($data, 0, $p + 1)));
        if ($p + 1 < strlen($data)) {
            stream_bucket_prepend($in, stream_bucket_new($this->stream, substr($data, $p + 1)));
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("lines", "lines");
$f = __DIR__ . "/user_filter_pass_on_leftover_read.bin";
file_put_contents($f, "abc\ndef\nghi\n");
$fp = fopen($f, 'r');
stream_set_chunk_size($fp, 6);
stream_filter_append($fp, "lines", STREAM_FILTER_READ);
var_dump(stream_get_contents($fp));
fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_pass_on_leftover_read.bin");
?>
--EXPECTF--
Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s on line %d
string(10) "abc
f
ghi
"
