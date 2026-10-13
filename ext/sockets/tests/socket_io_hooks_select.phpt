--TEST--
IO hooks: socket_select() polls first and waits with one Any operation over the Sockets
--EXTENSIONS--
sockets
--FILE--
<?php
include __DIR__ . '/../../standard/tests/streams/hooks/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $anys = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Any) {
            $members = $op->getOperations();
            $polls = array_filter($members, fn ($m) => $m instanceof \Io\Operation\Poll);
            $this->anys[] = count($polls) . '+' . (count($members) - count($polls))
                . ' ' . implode(',', array_unique(array_map(fn ($m) => get_class($m->getHandle()), $polls)));
        }
        return parent::run($op);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

socket_create_pair(PHP_OS_FAMILY === 'Windows' ? AF_INET : AF_UNIX, SOCK_STREAM, 0, $p1);
socket_create_pair(PHP_OS_FAMILY === 'Windows' ? AF_INET : AF_UNIX, SOCK_STREAM, 0, $p2);
[$a, $b] = $p1;
[$c, $d] = $p2;

$scheduler->spawn(function () use ($a, $c) {
    // Nothing readable: the select times out through the Timer member
    $r = [$a, $c]; $w = null; $e = null;
    $start = hrtime(true);
    var_dump(socket_select($r, $w, $e, 0, 50000), $r);
    var_dump((hrtime(true) - $start) / 1e6 >= 40);

    // Both readable by now, and the writable side: answered by the poll before the Any
    usleep(150000);
    $r = [$a, $c]; $w = [$a]; $e = null;
    var_dump(socket_select($r, $w, $e, 1), count($r), $w === [$a]);
    var_dump(socket_read($a, 10), socket_read($c, 10));

    // A blocking select woken by the other fiber
    socket_write($a, "go");
    $r = [$a]; $w = null; $e = null;
    var_dump(socket_select($r, $w, $e, null), $r === [$a], socket_read($a, 10));
});
$scheduler->spawn(function () use ($b, $d) {
    usleep(70000);
    socket_write($b, "one");
    socket_write($d, "two");
    socket_read($b, 2);
    // Let the select block before the wakeup: a direct queue sends without a fiber switch
    usleep(20000);
    socket_write($b, "three");
});
$scheduler->loop();
Io\Hooks\set_hooks(null);
var_dump($scheduler->anys);
?>
--EXPECT--
int(0)
array(0) {
}
bool(true)
int(3)
int(2)
bool(true)
string(3) "one"
string(3) "two"
int(1)
bool(true)
string(5) "three"
array(2) {
  [0]=>
  string(24) "2+1 SocketPollWeakHandle"
  [1]=>
  string(24) "1+0 SocketPollWeakHandle"
}
