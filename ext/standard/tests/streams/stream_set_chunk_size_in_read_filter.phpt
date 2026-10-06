--TEST--
stream_set_chunk_size() called by a read filter
--FILE--
<?php
class Grow extends php_user_filter {
    private int $seen = 0;

    public function filter($in, $out, &$consumed, bool $closing): int {
        stream_set_chunk_size($this->stream, 4000000);

        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            $this->seen += $bucket->datalen;
        }

        if ($closing) {
            stream_bucket_append($out, stream_bucket_new($this->stream, (string) $this->seen));
            return PSFS_PASS_ON;
        }

        return PSFS_FEED_ME;
    }
}

stream_filter_register('grow', 'Grow');

$file = __DIR__ . '/stream_set_chunk_size_in_read_filter.tmp';
file_put_contents($file, str_repeat('x', 65536));

$handle = fopen($file, 'r');
stream_filter_append($handle, 'grow', STREAM_FILTER_READ);
var_dump(fread($handle, 4096));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stream_set_chunk_size_in_read_filter.tmp');
?>
--EXPECT--
string(5) "65536"
