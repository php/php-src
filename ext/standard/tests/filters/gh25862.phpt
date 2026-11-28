--TEST--
GH-25862 (Heap buffer overflow in _php_stream_fill_read_buffer() when a read filter increases the stream chunk size)
--CREDITS--
matyapiro31
--FILE--
<?php
class ChunkSizeFilter extends php_user_filter {
    private int $calls = 0;

    public function filter($in, $out, &$consumed, $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            if ($this->calls > 0) {
                stream_bucket_append($out, $bucket);
            }
        }
        if (++$this->calls === 1) {
            stream_set_chunk_size($this->stream, 65536);
            return PSFS_FEED_ME;
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register('chunk_size_filter', 'ChunkSizeFilter');

$fp = fopen('php://memory', 'w+');
fwrite($fp, str_repeat('a', 128) . str_repeat('b', 65536));
rewind($fp);
stream_set_chunk_size($fp, 128);
stream_filter_append($fp, 'chunk_size_filter', STREAM_FILTER_READ);
var_dump(fread($fp, 10));
fclose($fp);
?>
--EXPECT--
string(10) "bbbbbbbbbb"
