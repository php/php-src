--TEST--
Io\Poll: a watcher on a persistent stream is unregistered at shutdown (GH-22844)
--SKIPIF--
<?php
if (!Io\Poll\Backend::Poll->isAvailable()) {
    die("skip Poll backend not available\n");
}
$srv = @stream_socket_server("tcp://127.0.0.1:0", $e1, $e2);
if (!$srv) {
    die("skip cannot bind loopback listener\n");
}
$addr = stream_socket_get_name($srv, false);
$p = @pfsockopen("tcp://" . $addr, -1, $en, $es, 1);
if (!$p) {
    die("skip pfsockopen unavailable\n");
}
?>
--FILE--
<?php
// A persistent stream outlives the request that watched it. The watcher (a
// request object) must unregister from the persistent stream when it is freed
// at shutdown; otherwise poll_watchers keeps a dangling pointer into freed
// request memory. Run under ASAN/valgrind to catch the cross-request UAF.
$srv = stream_socket_server("tcp://127.0.0.1:0", $e1, $e2);
$addr = stream_socket_get_name($srv, false);
$p = pfsockopen("tcp://" . $addr, -1, $en, $es, 1);

$ctx = new Io\Poll\Context(Io\Poll\Backend::Poll);
$watcher = $ctx->add(new StreamPollHandle($p), [Io\Poll\Event::Write]);

echo "watched: "; var_dump(count($ctx->wait(0, 0)) >= 0);
echo "done\n";
// $watcher / $ctx / persistent $p are intentionally left for shutdown teardown.
?>
--EXPECT--
watched: bool(true)
done
