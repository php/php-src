--TEST--
stream_filter_remove() while the same user filter is suspended in several Fibers that finish in reverse order
--FILE--
<?php
class Suspender extends php_user_filter {
    public function filter($in, $out, &$consumed, bool $closing): int {
        while ($bucket = stream_bucket_make_writeable($in)) {
            $consumed += $bucket->datalen;
            stream_bucket_append($out, $bucket);
        }

        if (Fiber::getCurrent()) {
            Fiber::suspend();
        }

        return PSFS_PASS_ON;
    }
}

stream_filter_register('suspender', 'Suspender');

$stream = fopen('php://memory', 'w+');
$filter = stream_filter_append($stream, 'suspender', STREAM_FILTER_WRITE);

$fiber1 = new Fiber(fn() => fwrite($stream, 'one'));
$fiber2 = new Fiber(fn() => fwrite($stream, 'two'));
$fiber1->start();
$fiber2->start();
$fiber2->resume();
var_dump($fiber2->getReturn());

var_dump(stream_filter_remove($filter));

$fiber1->resume();
var_dump($fiber1->getReturn());

var_dump(stream_filter_remove($filter));

rewind($stream);
var_dump(stream_get_contents($stream));
?>
--EXPECTF--
int(3)

Warning: stream_filter_remove(): Unable to remove a filter while it is running in %s on line %d
bool(false)
int(3)
bool(true)
string(6) "twoone"
