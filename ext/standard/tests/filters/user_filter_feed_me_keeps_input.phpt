--TEST--
User filter returning PSFS_FEED_ME keeps buckets it put back on the input brigade
--FILE--
<?php
class defer extends php_user_filter {
    private int $calls = 0;
    public function filter($in, $out, &$consumed, bool $closing): int {
        $this->calls++;
        $buckets = [];
        while ($b = stream_bucket_make_writeable($in)) { $buckets[] = $b; }
        if ($this->calls < 3) {
            foreach ($buckets as $b) { stream_bucket_prepend($in, $b); }
            return PSFS_FEED_ME;
        }
        foreach ($buckets as $b) { stream_bucket_append($out, $b); }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("defer", "defer");
$f = __DIR__ . "/user_filter_feed_me_keeps_input.bin";
file_put_contents($f, str_repeat("a", 8192) . str_repeat("b", 8192) . str_repeat("c", 8192));
$fp = fopen($f, 'r');
stream_filter_append($fp, "defer", STREAM_FILTER_READ);
var_dump(strlen(stream_get_contents($fp)));
fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_feed_me_keeps_input.bin");
?>
--EXPECT--
int(24576)
