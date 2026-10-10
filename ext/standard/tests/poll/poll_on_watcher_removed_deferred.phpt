--TEST--
Io\Poll: onWatcherRemoved runs on the next wait(), never from the close path
--FILE--
<?php
require_once __DIR__ . '/poll.inc';
use Io\Poll\Event;

$zero = Time\Duration::fromSeconds(0);

echo "-- callback drops the other context\n";
[$r, $w] = pt_new_socket_pair();
$c1 = pt_new_stream_poll();
$c2 = pt_new_stream_poll();
$h = new StreamPollHandle($r);
$c1->add($h, [Event::Read], 'c1');
$c2->add($h, [Event::Read], 'c2');
unset($h);
$c1->onWatcherRemoved(function ($wt) { echo "c1: ", $wt->getData(), "\n"; unset($GLOBALS['c2']); });
$c2->onWatcherRemoved(function ($wt) { echo "c2: ", $wt->getData(), "\n"; unset($GLOBALS['c1']); });
fclose($r);
echo "closed\n";
$c1->wait($zero);
var_dump(isset($c2));

echo "-- callback removes a watcher of the same stream\n";
[$r, $w] = pt_new_socket_pair();
$c1 = pt_new_stream_poll();
$c2 = pt_new_stream_poll();
$h = StreamPollWeakHandle::create($r);
$w1 = $c1->add($h, [Event::Read]);
$w2 = $c2->add($h, [Event::Read]);
$c1->onWatcherRemoved(function ($wt) use ($w2) { echo "c1 removal\n"; $w2->remove(); });
fclose($r);
$c1->wait($zero);
var_dump($w1->isActive(), $w2->isActive());

echo "-- exceptions come from wait(), the rest stays queued\n";
$c = pt_new_stream_poll();
[$r1, $w1] = pt_new_socket_pair();
[$r2, $w2] = pt_new_socket_pair();
$c->add(new StreamPollHandle($r1), [Event::Read], 'one');
$c->add(new StreamPollHandle($r2), [Event::Read], 'two');
$c->onWatcherRemoved(function ($wt) { echo "cb ", $wt->getData(), "\n"; throw new Exception($wt->getData()); });
fclose($r1);
fclose($r2);
echo "closed\n";
foreach ([1, 2, 3] as $i) {
    try {
        var_dump(count($c->wait($zero)));
    } catch (Exception $e) {
        echo "caught ", $e->getMessage(), "\n";
    }
}

echo "-- queued removals are dropped with the context\n";
[$r, $w] = pt_new_socket_pair();
$c = pt_new_stream_poll();
$c->add(StreamPollWeakHandle::create($r), [Event::Read]);
$c->onWatcherRemoved(function ($wt) { echo "never\n"; });
fclose($r);
unset($c);

echo "-- GC of the stream runs no callback\n";
class Holder { public $self; public $s; }
$c = pt_new_stream_poll();
$c->onWatcherRemoved(function ($wt) { echo "removed, stream ", var_export($wt->getHandle()->getStream(), true), "\n"; });
(function () use ($c) {
    [$r, $w] = pt_new_socket_pair();
    $o = new Holder;
    $o->self = $o;
    $o->s = $r;
    $c->add(StreamPollWeakHandle::create($r), [Event::Read]);
})();
gc_collect_cycles();
echo "collected\n";
$c->wait($zero);
echo "done\n";
?>
--EXPECT--
-- callback drops the other context
closed
c1: c1
bool(false)
-- callback removes a watcher of the same stream
c1 removal
bool(false)
bool(false)
-- exceptions come from wait(), the rest stays queued
closed
cb one
caught one
cb two
caught two
int(0)
-- queued removals are dropped with the context
-- GC of the stream runs no callback
collected
removed, stream NULL
done
