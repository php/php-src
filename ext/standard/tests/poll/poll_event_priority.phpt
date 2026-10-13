--TEST--
Io\Poll: Event::Priority reports TCP urgent data where the backend supports it
--EXTENSIONS--
sockets
--SKIPIF--
<?php
require_once __DIR__ . '/poll.inc';
if (!pt_new_stream_poll()->getBackend()->supportsPriority()) die("skip Priority is not supported by this backend");
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

$ctx = pt_new_stream_poll();
[$client, $server] = pt_new_tcp_socket_pair();

$watcher = $ctx->add(new StreamPollHandle($server), [Io\Poll\Event::Priority], "server");
echo "Events count: ", count($ctx->wait(Time\Duration::fromSeconds(0))), "\n";

$sock = socket_import_stream($client);
socket_send($sock, "!", 1, MSG_OOB);
$events = $ctx->wait(Time\Duration::fromMicroseconds(200000));
echo "Events count: ", count($events), "\n";
var_dump($events[0]->hasTriggered(Io\Poll\Event::Priority));
var_dump(in_array(Io\Poll\Event::Priority, $events[0]->getWatchedEvents(), true));
$watcher->remove();
echo "done\n";
?>
--EXPECTF--
Events count: 0
Events count: 1
bool(true)
bool(true)
done
