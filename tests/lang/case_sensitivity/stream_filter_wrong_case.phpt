--TEST--
stream_filter_register() with wrong-case class name fails with wrong case
--FILE--
<?php
class MyFilter extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $bucket->data = strtoupper($bucket->data);
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

try {
    stream_filter_register("my.upper", "MYFILTER");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
stream_filter_register(): Argument #2 ($class) must be a valid class name, MYFILTER given
