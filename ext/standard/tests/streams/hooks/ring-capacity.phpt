--TEST--
Io\Ring\Engine: the smallest ring takes more operations than it has entries and keeps their deadlines
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

$n = 100;
$pairs = [];
$results = [];
for ($i = 0; $i < $n; $i++) {
    $pairs[$i] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
}
fwrite($pairs[7][1], "seven");

// The smallest ring: an operation and its linked timeout still fit
$scheduler = new Scheduler(new Io\Ring\Engine(1));
Io\Hooks\set_hooks($scheduler);
for ($i = 0; $i < $n; $i++) {
    $scheduler->spawn(function () use ($i, $pairs, &$results) {
        stream_set_timeout($pairs[$i][0], 0, 200000);
        $r = fread($pairs[$i][0], 10);
        $results[] = var_export($r, true) . " timed_out=" . var_export(stream_get_meta_data($pairs[$i][0])['timed_out'], true);
    });
}
$scheduler->loop();
$counts = array_count_values($results);
ksort($counts);
print_r($counts);

// Operations still pending when the script ends
$ring = new Io\Ring\Engine(1);
Io\Hooks\set_hooks(new class($ring) implements Io\Hooks\Hooks {
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return Scheduler::capabilities($this->ring); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        $this->ring->submit($op);
        return Fiber::suspend();
    }
});
$fibers = [];
for ($i = 0; $i < $n; $i++) {
    stream_set_timeout($pairs[$i][0], -1);
    $fibers[$i] = new Fiber(function () use ($i, $pairs) { fread($pairs[$i][0], 10); });
    $fibers[$i]->start();
}
var_dump($ring->countPending());
echo "end\n";
?>
--EXPECT--
Array
(
    ['seven' timed_out=false] => 1
    [false timed_out=true] => 99
)
int(100)
end
