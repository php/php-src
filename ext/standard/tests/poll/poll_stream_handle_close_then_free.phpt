--TEST--
Io\Poll: StreamPollHandle cleanup is safe when the stream is closed first
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);

// Close the underlying streams before the watcher and handle are freed.
fclose($r);
fclose($w);

unset($watcher, $ctx);
gc_collect_cycles();
echo "ok\n";
?>
--EXPECT--
ok
