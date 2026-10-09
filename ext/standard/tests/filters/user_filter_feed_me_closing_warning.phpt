--TEST--
Input kept back by a PSFS_FEED_ME filter on the closing call is discarded with a warning
--FILE--
<?php
class defer extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        $buckets = [];
        while ($b = stream_bucket_make_writeable($in)) { $buckets[] = $b; $consumed += $b->datalen; }
        foreach (array_reverse($buckets) as $b) { stream_bucket_prepend($in, $b); }
        return PSFS_FEED_ME;
    }
}
stream_filter_register("defer", "defer");
$f = __DIR__ . "/user_filter_feed_me_closing_warning.bin";
$fp = fopen($f, 'w');
stream_filter_append($fp, "defer", STREAM_FILTER_WRITE);
var_dump(fwrite($fp, "hello"));
fclose($fp);
var_dump(file_get_contents($f));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/user_filter_feed_me_closing_warning.bin");
?>
--EXPECTF--
int(5)

Warning: fclose(): Unprocessed filter buckets remaining on input brigade in %s on line %d
string(0) ""
