--TEST--
Io\Poll: cycle collector reclaims a Watcher referenced through its own data
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

class Canary {
    public $ref;
    public function __destruct() {
        echo "Canary freed\n";
    }
}

list($r, $w) = pt_new_socket_pair();
$ctx = pt_new_stream_poll();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);

$c = new Canary();
$c->ref = $watcher;         // Canary -> Watcher
$watcher->modifyData($c);   // Watcher->data -> Canary (cycle)

unset($ctx, $watcher, $c, $r, $w);

echo "before gc\n";
gc_collect_cycles();
echo "after gc\n";
?>
--EXPECT--
before gc
Canary freed
after gc
