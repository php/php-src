--TEST--
stream_filter_remove() of a user filter that a Fiber entered during its flush
--FILE--
<?php
class Reader extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        global $stream, $fiber;

        if ($closing && !$fiber) {
            $fiber = new Fiber(fn() => fread($stream, 1));
            $fiber->start();
        }

        if ($fiber && Fiber::getCurrent() === $fiber) {
            Fiber::suspend();
        }

        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }

        return PSFS_PASS_ON;
    }
}

stream_filter_register('reader', 'Reader');

$stream = fopen('php://memory', 'w+');
fwrite($stream, 'abc');
rewind($stream);
$filter = stream_filter_append($stream, 'reader', STREAM_FILTER_READ);

var_dump(stream_filter_remove($filter));
$fiber->resume();
var_dump($fiber->getReturn());
var_dump(stream_filter_remove($filter));
?>
--EXPECTF--
Warning: stream_filter_remove(): Unable to remove a filter while it is running in %s on line %d
bool(false)
string(1) "a"
bool(true)
