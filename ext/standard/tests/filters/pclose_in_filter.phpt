--TEST--
pclose() of the stream from its user filter's callback
--FILE--
<?php
class Closer extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        global $fp;
        if (!$closing) {
            var_dump(pclose($fp));
        }
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register('closer', 'Closer');
$fp = fopen('php://memory', 'w+');
stream_filter_append($fp, 'closer', STREAM_FILTER_WRITE);
fwrite($fp, 'abc');
var_dump(pclose($fp));
?>
--EXPECTF--
Warning: pclose(): %d is not a valid stream resource in %s on line %d
int(-1)
int(0)
