--TEST--
stream_filter_remove() keeps readbuflen in sync when growing the read buffer
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
            stream_bucket_append($out, stream_bucket_new($this->stream, str_repeat('F', 20000)));
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register('closing-suffix', ClosingSuffixFilter::class);

/* The file must be larger than one chunk so that the stream is not at EOF
 * when the filter is removed and a refill of the read buffer happens later. */
$file = __DIR__ . '/stream_filter_remove_grow_read_buffer.tmp';
$content = str_repeat('a', 100) . str_repeat('b', 100000);
file_put_contents($file, $content);

$stream = fopen($file, 'r');
$filter = stream_filter_append($stream, 'closing-suffix', STREAM_FILTER_READ);
var_dump(fread($stream, 100) === str_repeat('a', 100));
/* The flushed 20000 bytes exceed the remaining read buffer capacity, so the buffer is grown */
var_dump(stream_filter_remove($filter));
$rest = stream_get_contents($stream);
var_dump(strlen($rest));
/* Data already buffered before the flush comes first, then the flushed data, then the rest of the file */
var_dump(preg_match('/^b*F{20000}b*$/', $rest));
var_dump(substr_count($rest, 'b'));
var_dump(feof($stream));
fclose($stream);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stream_filter_remove_grow_read_buffer.tmp');
?>
--EXPECT--
bool(true)
bool(true)
int(120000)
int(1)
int(100000)
bool(true)
