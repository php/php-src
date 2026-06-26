--TEST--
GH-22063 (Stream filter chain UAF via self-removal during callback)
--FILE--
<?php
class SelfRemovingFilter extends php_user_filter {
    public $stream;
    public static ?string $key = null;
    public static ?string $other_key = null;
    public static bool $on_closing_only = false;

    public function filter($in, $out, &$consumed, $closing): int
    {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }
        if (self::$key !== null && (!self::$on_closing_only || $closing)) {
            $res = $GLOBALS[self::$key];
            $other = self::$other_key !== null ? $GLOBALS[self::$other_key] : null;
            self::$key = null;
            self::$other_key = null;
            var_dump(stream_filter_remove($res));
            if ($other) {
                var_dump(stream_filter_remove($other));
            }
        }
        return PSFS_PASS_ON;
    }
}

stream_filter_register('self-removing', SelfRemovingFilter::class);

echo "write side:\n";
$f = fopen('php://memory', 'r+');
SelfRemovingFilter::$key = 'write_res';
SelfRemovingFilter::$on_closing_only = false;
$GLOBALS['write_res'] = stream_filter_append($f, 'self-removing', STREAM_FILTER_WRITE);
fwrite($f, 'hello');
fwrite($f, ' world');
rewind($f);
echo stream_get_contents($f), "\n";

echo "cross-filter side:\n";
$f = fopen('php://memory', 'r+');
SelfRemovingFilter::$key = 'cross_res';
SelfRemovingFilter::$other_key = 'cross_other';
SelfRemovingFilter::$on_closing_only = false;
$GLOBALS['cross_res'] = stream_filter_append($f, 'self-removing', STREAM_FILTER_WRITE);
$GLOBALS['cross_other'] = stream_filter_append($f, 'string.rot13', STREAM_FILTER_WRITE);
fwrite($f, 'hello world');
rewind($f);
echo stream_get_contents($f), "\n";

echo "read side:\n";
$f = fopen('php://memory', 'r+');
fwrite($f, 'abcdefghij');
rewind($f);
SelfRemovingFilter::$key = 'read_res';
SelfRemovingFilter::$on_closing_only = false;
$GLOBALS['read_res'] = stream_filter_append($f, 'self-removing', STREAM_FILTER_READ);
echo fread($f, 4), '|', fread($f, 6), "\n";

echo "closing-flush side:\n";
$f = fopen('php://memory', 'r+');
SelfRemovingFilter::$key = 'close_res';
SelfRemovingFilter::$on_closing_only = true;
$GLOBALS['close_res'] = stream_filter_append($f, 'self-removing', STREAM_FILTER_WRITE);
var_dump(stream_filter_remove($GLOBALS['close_res']));
?>
--EXPECTF--
write side:

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)
hello world
cross-filter side:

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)
uryyb jbeyq
read side:

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)
abcd|efghij
closing-flush side:

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)
bool(true)
