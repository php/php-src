--TEST--
IO hooks: a read on a non-blocking pipe is the plain syscall, never a wait
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') die('skip no non-blocking pipes on Windows');
?>
--FILE--
<?php
final class Recording implements Io\Hooks\Hooks
{
    public array $ops = [];
    public function getCapabilities(): array { return []; }
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ops[] = $op::class;
        return $op->complete(Io\CompletionStatus::Timeout);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

$proc = proc_open([PHP_BINARY, '-n', '-r', 'fgets(STDIN);'], [0 => ['pipe', 'r'], 1 => ['pipe', 'w']], $pipes);
stream_set_blocking($pipes[1], false);
$provider = new Recording();
Io\Hooks\set_hooks($provider);
var_dump(fread($pipes[1], 100));
Io\Hooks\set_hooks(null);
var_dump($provider->ops);
fclose($pipes[0]);
fclose($pipes[1]);
proc_close($proc);
?>
--EXPECT--
string(0) ""
array(0) {
}
