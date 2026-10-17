--TEST--
stream_filter_remove() of a user filter while it is running
--FILE--
<?php
class Remover extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }

        if ($closing && isset($GLOBALS['remove'])) {
            $filter = $GLOBALS['remove'];
            unset($GLOBALS['remove']);
            var_dump(stream_filter_remove($filter));
        }

        if (Fiber::getCurrent()) {
            Fiber::suspend();
        }

        return PSFS_PASS_ON;
    }
}

stream_filter_register('remover', 'Remover');

echo "From the filter itself:\n";
$stream = fopen('php://memory', 'w+');
$filter = stream_filter_append($stream, 'remover', STREAM_FILTER_WRITE);
fwrite($stream, 'hello');
$GLOBALS['remove'] = $filter;
var_dump(stream_filter_remove($filter));

echo "While the filter is suspended in a Fiber:\n";
$stream = fopen('php://memory', 'w+');
$filter = stream_filter_append($stream, 'remover', STREAM_FILTER_WRITE);
$fiber = new Fiber(fn() => fwrite($stream, 'hello'));
$fiber->start();
var_dump(stream_filter_remove($filter));
$fiber->resume();
var_dump($fiber->getReturn());
var_dump(stream_filter_remove($filter));
?>
--EXPECTF--
From the filter itself:

Warning: stream_filter_remove(): Unable to remove a filter while it is running in %s on line %d
bool(false)
bool(true)
While the filter is suspended in a Fiber:

Warning: stream_filter_remove(): Unable to remove a filter while it is running in %s on line %d
bool(false)
int(5)
bool(true)
