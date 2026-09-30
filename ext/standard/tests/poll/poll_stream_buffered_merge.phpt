--TEST--
Io\Poll: buffered data is merged with descriptor events, one entry per watcher
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$a, $b] = pt_new_socket_pair();
[$c, $d] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$wa = pt_stream_poll_add($ctx, $a, [Io\Poll\Event::Read], 'a');
$wc = pt_stream_poll_add($ctx, $c, [Io\Poll\Event::Read], 'c');

// $a: buffered data and an idle socket; $c: data on the socket
fwrite($b, "first\nsecond\n");
var_dump(fgets($a));
fwrite($d, "x");

$events = $ctx->wait(Time\Duration::fromMilliseconds(100));
$seen = [];
foreach ($events as $e) {
    $seen[$e->getData()] = $e->hasTriggered(Io\Poll\Event::Read);
}
ksort($seen);
var_dump($seen);

// $a: buffered data and data on the socket at once: one entry, Read once
fwrite($b, "third\n");
var_dump(fread($c, 10));
$events = $ctx->wait(Time\Duration::fromMilliseconds(100));
var_dump(count($events), $events[0] === $wa, $events[0]->hasTriggered(Io\Poll\Event::Read));
var_dump(fgets($a), fgets($a));
var_dump(count($ctx->wait(Time\Duration::fromSeconds(0))));
?>
--EXPECT--
string(6) "first
"
array(2) {
  ["a"]=>
  bool(true)
  ["c"]=>
  bool(true)
}
string(1) "x"
int(1)
bool(true)
bool(true)
string(7) "second
"
string(6) "third
"
int(0)
