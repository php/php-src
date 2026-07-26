--TEST--
Io\Poll: closing a watched stream whose fd is duplicated removes the registration (GH-22844)
--SKIPIF--
<?php
if (!function_exists('proc_open')) {
    die("skip proc_open required\n");
}
if (!Io\Poll\Backend::Epoll->isAvailable()) {
    die("skip Epoll backend required\n");
}
?>
--FILE--
<?php
require_once __DIR__ . '/poll.inc';

// epoll keeps the interest alive after fclose() while a duplicated descriptor
// (here inherited by a child) keeps the open file description open. The
// close-time callback must drop the interest while the fd is still valid so
// wait() never returns the retired (freed) watcher.
$ctx = new Io\Poll\Context(Io\Poll\Backend::Epoll);
[$r, $w] = pt_new_socket_pair();
$watcher = $ctx->add(new StreamPollHandle($r), [Io\Poll\Event::Read]);

$proc = proc_open('sleep 1', [0 => $r], $pipes);
fclose($r);
unset($watcher);
gc_collect_cycles();

fwrite($w, "ping");
echo "events: "; var_dump(count($ctx->wait(0, 0)));

proc_terminate($proc);
proc_close($proc);
echo "done\n";
?>
--EXPECT--
events: int(0)
done
