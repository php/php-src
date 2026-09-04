--TEST--
php_stream_write_filtered() discards output buckets a filter left behind on PSFS_FEED_ME
--FILE--
<?php
class feed_filter extends php_user_filter
{
    public function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += strlen($bucket->data);
        }
        stream_bucket_append($out, stream_bucket_new($this->stream, 'abandoned'));
        return PSFS_FEED_ME;
    }
}

stream_filter_register('feed', feed_filter::class);

$fp = fopen('php://memory', 'w+');
stream_filter_append($fp, 'feed', STREAM_FILTER_WRITE);
var_dump(fwrite($fp, 'one'));
fclose($fp);

echo "done\n";
?>
--EXPECT--
int(3)
done
