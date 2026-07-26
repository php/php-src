--TEST--
Io\Poll: closing a stream watched in multiple contexts retires every watcher (GH-22844)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

// One stream can carry watchers in several contexts; closing it must unregister
// all of them, not just one.
[$r, $w] = pt_new_socket_pair();
$handle = new StreamPollHandle($r);

$ctx1 = pt_new_stream_poll();
$ctx2 = pt_new_stream_poll();
$w1 = $ctx1->add($handle, [Io\Poll\Event::Read]);
$w2 = $ctx2->add($handle, [Io\Poll\Event::Read]);

fwrite($w, "ping");
fclose($r);
fclose($w);

echo "ctx1: "; var_dump(count($ctx1->wait(0, 0)));
echo "ctx2: "; var_dump(count($ctx2->wait(0, 0)));
echo "done\n";
?>
--EXPECT--
ctx1: int(0)
ctx2: int(0)
done
