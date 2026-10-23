--TEST--
stream_filter_remove() of another filter of the stream from a user filter's callback
--FILE--
<?php
class Remover extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        if (isset($GLOBALS['remove'])) {
            $filter = $GLOBALS['remove'];
            unset($GLOBALS['remove']);
            var_dump(stream_filter_remove($filter));
        }

        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }

        return PSFS_PASS_ON;
    }
}

stream_filter_register('remover', 'Remover');

$stream = fopen('php://memory', 'w+');
stream_filter_append($stream, 'remover', STREAM_FILTER_WRITE);
$GLOBALS['remove'] = stream_filter_append($stream, 'string.toupper', STREAM_FILTER_WRITE);
var_dump(fwrite($stream, 'hello'));

rewind($stream);
var_dump(stream_get_contents($stream));
?>
--EXPECT--
bool(true)
int(5)
string(5) "hello"
