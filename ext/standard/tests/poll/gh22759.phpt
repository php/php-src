--TEST--
GH-22759 (Io\Poll: re-adding a removed handle keeps the new watcher tracked)
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

[$r, $w] = pt_new_socket_pair();
$context = pt_new_stream_poll();
$handle = new StreamPollHandle($r);

$first = $context->add($handle, [Io\Poll\Event::Read]);
$first->remove();

$second = $context->add($handle, [Io\Poll\Event::Read]);

unset($context);
gc_collect_cycles();

var_dump($second->isActive());

try {
    $second->remove();
} catch (Io\Poll\InactiveWatcherException $e) {
    echo $e->getMessage(), "\n";
}

echo "done\n";
?>
--EXPECT--
bool(false)
Cannot remove inactive watcher
done
