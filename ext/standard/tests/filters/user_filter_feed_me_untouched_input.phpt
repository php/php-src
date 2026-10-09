--TEST--
User filter returning PSFS_FEED_ME without touching the input brigade sees the original data again
--FILE--
<?php
class defer extends php_user_filter {
    private int $calls = 0;
    public function filter($in, $out, &$consumed, bool $closing): int {
        $this->calls++;
        if ($this->calls < 3) {
            return PSFS_FEED_ME;
        }
        while ($b = stream_bucket_make_writeable($in)) {
            stream_bucket_append($out, $b);
        }
        return PSFS_PASS_ON;
    }
}
stream_filter_register("defer", "defer");
$f = __DIR__ . "/user_filter_feed_me_untouched_input.bin";
file_put_contents($f, str_repeat("a", 8192) . str_repeat("b", 8192) . str_repeat("c", 8192));
$fp = fopen($f, 'r');
stream_filter_append($fp, "defer", STREAM_FILTER_READ);
$data = stream_get_contents($fp);
fclose($fp);
var_dump(strlen($data), substr_count($data, "a"), substr_count($data, "b"), substr_count($data, "c"));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_feed_me_untouched_input.bin");
?>
--EXPECT--
int(24576)
int(8192)
int(8192)
int(8192)
