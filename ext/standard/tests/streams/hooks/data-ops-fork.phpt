--TEST--
IO hooks: pcntl_fork() refuses while an operation is in flight
--EXTENSIONS--
pcntl
--FILE--
<?php
[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);

Io\Hooks\set_hooks(new class implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return []; }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        try {
            pcntl_fork();
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        return $op->complete(Io\CompletionStatus::Timeout);
    }
});
var_dump(fread($r, 10));
Io\Hooks\set_hooks(null);

// Outside an operation forking works
$pid = pcntl_fork();
if ($pid === 0) {
    exit(0);
}
pcntl_waitpid($pid, $status);
var_dump($pid > 0);
?>
--EXPECT--
Cannot fork while IO operations are in flight
bool(false)
bool(true)
