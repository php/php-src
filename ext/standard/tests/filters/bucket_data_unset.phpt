--TEST--
unset(StreamBucket::$data) in filter callback must not crash when bucket is re-attached
--FILE--
<?php
class MyFilter extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            unset($bucket->data);
            stream_bucket_prepend($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("myfilter", "MyFilter");
$fp = fopen("php://temp", "w+");
fwrite($fp, str_repeat("A", 100));
rewind($fp);
stream_filter_append($fp, "myfilter");
try {
    var_dump(stream_get_contents($fp));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo "DONE\n";
--EXPECT--
Error: Typed property StreamBucket::$data must not be accessed before initialization
DONE
