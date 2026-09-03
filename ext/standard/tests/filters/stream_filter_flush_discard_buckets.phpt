--TEST--
php_stream_filter_flush() discards buckets a filter left behind on PSFS_FEED_ME / PSFS_ERR_FATAL
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

class fatal_filter extends php_user_filter
{
    public function filter($in, $out, &$consumed, $closing): int
    {
        stream_bucket_append($out, stream_bucket_new($this->stream, 'abandoned'));
        return PSFS_ERR_FATAL;
    }
}

stream_filter_register('feed', feed_filter::class);
stream_filter_register('fatal', fatal_filter::class);

foreach (['feed', 'fatal'] as $name) {
    $fp = fopen('php://memory', 'w+');
    $filter = stream_filter_append($fp, $name, STREAM_FILTER_WRITE);
    var_dump(stream_filter_remove($filter));
    fclose($fp);
}

echo "done\n";
?>
--EXPECTF--
bool(true)

Warning: stream_filter_remove(): Unable to flush filter, not removing in %s on line %d
bool(false)
done
