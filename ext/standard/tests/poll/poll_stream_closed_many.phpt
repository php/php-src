--TEST--
Io\Poll: many watchers closed before removal leave the context consistent
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();

list($rk, $wk) = pt_new_socket_pair();
$keep = $ctx->add(new StreamPollHandle($rk), [Io\Poll\Event::Read], "keep");

list($ro, $wo) = pt_new_socket_pair();
$oneshot = $ctx->add(new StreamPollHandle($ro), [Io\Poll\Event::Read, Io\Poll\Event::OneShot], "oneshot");
fwrite($wo, "x");
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
var_dump(count($events), $events[0]->getData());

for ($i = 0; $i < 200; $i++) {
    list($r, $w) = pt_new_socket_pair();
    $watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);
    fclose($r);
    $watcher->remove();
    fclose($w);
}

// The fired one-shot watcher stays disarmed, the kept one is still watched
fwrite($wk, "y");
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
var_dump(count($events), $events[0]->getData());
var_dump($keep->isActive(), $oneshot->isActive());

$oneshot->modifyEvents([Io\Poll\Event::Read, Io\Poll\Event::OneShot]);
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
var_dump(count($events));

list($r, $w) = pt_new_socket_pair();
$new = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read], "new");
fwrite($w, "z");
$events = $ctx->wait(Time\Duration::fromMicroseconds(100000));
var_dump(count($events));
var_dump($oneshot->isActive());
?>
--EXPECT--
int(1)
string(7) "oneshot"
int(1)
string(4) "keep"
bool(true)
bool(true)
int(2)
int(2)
bool(true)
