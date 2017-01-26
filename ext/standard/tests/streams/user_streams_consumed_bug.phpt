--TEST--
Testing user filter on streams
--FILE--
<?php
class Intercept extends php_user_filter
{
    public static $cache = '';
    public function filter($in, $out, &$consumed, $closing)
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            self::$cache .= $bucket->data;
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

stream_filter_register("intercept_filter", "Intercept");
stream_filter_append(STDOUT, "intercept_filter");

$out = fwrite(STDOUT, "Goodbye\n");
var_dump($out);
--EXPECTF--
Hello
int(6)
Goodbye
int(8)

