--TEST--
GH-22063 (stream filter self-removal denied during a seek() callback)
--FILE--
<?php
class SeekRemover extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing): int {
        while ($b = stream_bucket_make_writeable($in)) {
            stream_bucket_append($out, $b);
        }
        return PSFS_PASS_ON;
    }
    public function seek(int $offset, int $whence, int $chain): bool {
        // Removing the filter that is mid-seek must be denied, not a UAF.
        var_dump(stream_filter_remove($GLOBALS['f']));
        return true;
    }
}
stream_filter_register('seekremover', SeekRemover::class);

$fp = fopen('php://memory', 'r+');
fwrite($fp, 'hello world data here');
rewind($fp);
$GLOBALS['f'] = stream_filter_append($fp, 'seekremover', STREAM_FILTER_READ);
var_dump(fread($fp, 4));
var_dump(fseek($fp, 0));
echo "done\n";
?>
--EXPECTF--
string(4) "hell"

Warning: stream_filter_remove(): Cannot remove filter while it is being applied in %s on line %d
bool(false)
int(0)
done
