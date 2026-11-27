--TEST--
Input kept by a PSFS_FEED_ME filter is re-presented to that filter only, not to the filters before it
--FILE--
<?php
class defer extends php_user_filter {
    private int $calls = 0;
    public function filter($in, $out, &$consumed, bool $closing): int {
        $this->calls++;
        $buckets = [];
        while ($b = stream_bucket_make_writeable($in)) { $buckets[] = $b; }
        if ($this->calls < 3) {
            foreach (array_reverse($buckets) as $b) { stream_bucket_prepend($in, $b); }
            return PSFS_FEED_ME;
        }
        foreach ($buckets as $b) { stream_bucket_append($out, $b); }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("defer", "defer");
$f = __DIR__ . "/user_filter_feed_me_chained.bin";
file_put_contents($f, str_repeat("a", 8192) . str_repeat("b", 8192) . str_repeat("c", 8192));
$fp = fopen($f, 'r');
stream_filter_append($fp, "string.rot13", STREAM_FILTER_READ);
stream_filter_append($fp, "defer", STREAM_FILTER_READ);
$data = stream_get_contents($fp);
fclose($fp);
var_dump($data === str_repeat("n", 8192) . str_repeat("o", 8192) . str_repeat("p", 8192));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_feed_me_chained.bin");
?>
--EXPECT--
bool(true)
