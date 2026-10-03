--TEST--
Io\Ring\Engine: with DirectData a read or write reaches the ring even when the socket is ready
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (!in_array(Io\Hooks\Capability::DirectData, (new Io\Ring\Engine())->getSupportedHookCapabilities(), true)) {
    die("skip the backend does not support DirectData");
}
?>
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

final class Tracing extends Scheduler
{
    public array $seen = [];

    public function run(\Io\Operation $op): \Io\Completion
    {
        $this->seen[] = $op::class;
        return parent::run($op);
    }
}

foreach ([[], [Io\Hooks\Capability::DirectData]] as $caps) {
    [$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
    $scheduler = new Tracing(new Io\Ring\Engine(), $caps);
    Io\Hooks\set_hooks($scheduler);
    $scheduler->spawn(function () use ($a, $b) {
        var_dump(fwrite($b, "ready"));
        var_dump(fread($a, 5));
    });
    $scheduler->loop();
    Io\Hooks\set_hooks(null);
    echo implode(",", $scheduler->seen) ?: "none", "\n";
}
?>
--EXPECT--
int(5)
string(5) "ready"
none
int(5)
string(5) "ready"
Io\Operation\Send,Io\Operation\Recv
