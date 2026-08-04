--TEST--
php_user_filter::seek receives chain identifier (read vs write)
--FILE--
<?php

class TrackingFilter extends php_user_filter
{
    public static array $log = [];

    public function filter($in, $out, &$consumed, bool $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }

    public function onCreate(): bool
    {
        return true;
    }

    public function onClose(): void {}

    public function seek(int $offset, int $whence, int $chain): bool
    {
        $name = match ($chain) {
            STREAM_FILTER_READ  => 'read',
            STREAM_FILTER_WRITE => 'write',
            default             => 'other',
        };
        self::$log[] = "$name:$offset:$whence";
        return true;
    }
}

stream_filter_register('test.tracking', 'TrackingFilter');

$file = __DIR__ . '/user_filter_seek_03.txt';
file_put_contents($file, "abcdefghij");

/* Read chain: seek-to-start should dispatch with STREAM_FILTER_READ */
TrackingFilter::$log = [];
$fp = fopen($file, 'r');
stream_filter_append($fp, 'test.tracking', STREAM_FILTER_READ);
fread($fp, 4);
fseek($fp, 0, SEEK_SET);
fclose($fp);
echo "Read chain log: " . implode(',', TrackingFilter::$log) . "\n";

/* Write chain: any seek should dispatch with STREAM_FILTER_WRITE */
TrackingFilter::$log = [];
$fp = fopen($file, 'w+');
stream_filter_append($fp, 'test.tracking', STREAM_FILTER_WRITE);
fwrite($fp, "xyz");
fseek($fp, 0, SEEK_SET);
fseek($fp, 5, SEEK_SET);
fclose($fp);
echo "Write chain log: " . implode(',', TrackingFilter::$log) . "\n";

unlink($file);

?>
--EXPECT--
Read chain log: read:0:0
Write chain log: write:0:0,write:5:0
