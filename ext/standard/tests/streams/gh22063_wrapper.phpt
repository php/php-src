--TEST--
GH-22063 (stream filter removal denied from a wrapper write during its own removal flush)
--FILE--
<?php
class HoldFilter extends php_user_filter {
    private string $buf = '';
    public function filter($in, $out, &$consumed, $closing): int {
        while ($b = stream_bucket_make_writeable($in)) {
            $this->buf .= $b->data;
            $consumed += $b->datalen;
        }
        if ($closing && $this->buf !== '') {
            stream_bucket_append($out, stream_bucket_new($this->stream, $this->buf));
            $this->buf = '';
            return PSFS_PASS_ON;
        }
        return PSFS_FEED_ME;
    }
}
class RemovingWrapper {
    public $context;
    public static bool $armed = false;
    public function stream_open($path, $mode, $options, &$opened) { return true; }
    public function stream_write($data) {
        if (self::$armed) {
            self::$armed = false;
            var_dump(stream_filter_remove($GLOBALS['w']));
        }
        return strlen($data);
    }
}
stream_filter_register('hold', HoldFilter::class);
stream_wrapper_register('rmw', RemovingWrapper::class);
$fp = fopen('rmw://x', 'w');
$GLOBALS['w'] = stream_filter_append($fp, 'hold', STREAM_FILTER_WRITE);
fwrite($fp, 'abc');
RemovingWrapper::$armed = true;
var_dump(stream_filter_remove($GLOBALS['w']));
echo "done\n";
?>
--EXPECTF--

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)
bool(true)
done
