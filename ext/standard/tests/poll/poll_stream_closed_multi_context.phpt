--TEST--
Io\Poll: closing a stream watched in several contexts leaves every watcher removable
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$handle = new StreamPollHandle($r);

$ctx1 = pt_new_stream_poll();
$ctx2 = pt_new_stream_poll();
$w1 = $ctx1->add($handle, [Io\Poll\Event::Read]);
$w2 = $ctx2->add($handle, [Io\Poll\Event::Read]);

fwrite($w, "ping");
fclose($r);

echo "ctx1 events count: ", count($ctx1->wait(Time\Duration::fromSeconds(0))), "\n";
echo "ctx2 events count: ", count($ctx2->wait(Time\Duration::fromSeconds(0))), "\n";
var_dump($w1->isActive());
var_dump($w2->isActive());

$w1->remove();
$w2->remove();
echo "removed\n";

fclose($w);
?>
--EXPECT--
ctx1 events count: 0
ctx2 events count: 0
bool(false)
bool(false)
removed
