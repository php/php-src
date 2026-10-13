--TEST--
IO hooks: stream_select() polls first and waits with one Any operation, reporting every ready stream
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $anys = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        if ($op instanceof \Io\Operation\Any) {
            $members = $op->getOperations();
            $this->anys[] = count(array_filter($members, fn ($m) => $m instanceof \Io\Operation\Poll))
                . '+' . count(array_filter($members, fn ($m) => $m instanceof \Io\Operation\Timer));
        }
        return parent::run($op);
    }
}

$scheduler = new Tracing();
Io\Hooks\set_hooks($scheduler);

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
[$c, $d] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

$scheduler->spawn(function () use ($a, $c) {
    // Nothing readable yet: the select times out
    $r = [$a, $c]; $w = null; $e = null;
    $start = hrtime(true);
    var_dump(stream_select($r, $w, $e, 0, 50000), $r);
    var_dump((hrtime(true) - $start) / 1e6 >= 40);

    // Both readable by now (the writer's sends are ops of their own on a
    // direct queue, so leave it a wide margin): both reported, the writable side too,
    // by the zero-timeout poll that precedes the Any, so no op reaches the provider
    usleep(150000);
    $r = [$a, $c]; $w = [$a]; $e = null;
    var_dump(stream_select($r, $w, $e, 1), count($r), $w === [$a]);
    var_dump(fread($a, 10), fread($c, 10));

    // Blocking select woken by the other fiber, told to go on
    fwrite($a, "go");
    $r = [$a]; $w = null; $e = null;
    var_dump(stream_select($r, $w, $e, null), $r === [$a], fread($a, 10));
});
$scheduler->spawn(function () use ($b, $d) {
    usleep(70000);
    fwrite($b, "one");
    fwrite($d, "two");
    fread($b, 2);
    // Let the select block before the wakeup: a direct queue sends without a fiber switch
    usleep(20000);
    fwrite($b, "three");
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
  string(3) "2+1"
  [1]=>
  string(3) "1+0"
}
