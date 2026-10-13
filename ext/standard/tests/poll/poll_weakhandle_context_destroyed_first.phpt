--TEST--
Io\Poll: Context destroyed before stream: watcher deactivated
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$handle = StreamPollWeakHandle::create($r);
$watcher = $ctx->add($handle, [Io\Poll\Event::Read]);

unset($ctx);
gc_collect_cycles();

var_dump($watcher->isActive());

fclose($r);
fclose($w);

echo "done\n";
?>
--EXPECT--
bool(false)
done
