--TEST--
IO hooks: completions a provider could not have produced are refused
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists('pcntl_sigtimedwait')) die('skip pcntl_sigtimedwait() not available');
?>
--FILE--
<?php

final class Fake implements Io\Hooks\Hooks
{
    public function __construct(private Closure $complete, private array $caps = []) {}
    public function getCapabilities(): array { return $this->caps; }
    public function run(Io\Operation $op): Io\Completion
    {
        return ($this->complete)($op) ?? $op->complete(Io\CompletionStatus::Unsupported);
    }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

function attempt(string $label, Closure $call, string $type, Closure $complete, array $caps = []): void
{
    Io\Hooks\set_hooks(new Fake(fn ($op) => $op instanceof $type ? $complete($op) : null, $caps));
    try {
        $r = $call();
        echo "$label: ", var_export($r, true), "\n";
    } catch (Error $e) {
        echo "$label: ", $e::class, ": ", $e->getMessage(), "\n";
    }
    Io\Hooks\set_hooks(null);
}

$file = __DIR__ . '/provider-invalid-results.txt';
file_put_contents($file, "hello");
$f = fopen($file, 'r+');
$files = [Io\Hooks\Capability::Files];
$direct = [Io\Hooks\Capability::DirectData];
$accept = [Io\Hooks\Capability::DirectAccept];

attempt('read', fn () => @fread($f, 100), Io\Operation\Read::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, 1 << 20), $files);
attempt('read eof', fn () => @fread($f, 100), Io\Operation\Read::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, 0), $files);
attempt('write', fn () => @fwrite($f, "0123456789"), Io\Operation\Write::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, $op->getLength() + 1), $files);
attempt('write short', fn () => @fwrite($f, "0123456789"), Io\Operation\Write::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, -1), $files);

[$a, $b] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
attempt('recv', fn () => @fread($a, 100), Io\Operation\Recv::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, 5), $direct);

$server = stream_socket_server('tcp://127.0.0.1:0');
attempt('accept', fn () => @stream_socket_accept($server, 5), Io\Operation\Accept::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, 1), $accept);

$pid = pcntl_fork();
if ($pid === 0) {
    exit(3);
}
attempt('waitpid', fn () => @pcntl_waitpid($pid, $status), Io\Operation\WaitPid::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, $op->getPid()));
var_dump(pcntl_waitpid($pid, $status) === $pid, pcntl_wexitstatus($status));

pcntl_sigprocmask(SIG_BLOCK, [SIGUSR1]);
attempt('sigwait', fn () => @pcntl_sigtimedwait([SIGUSR1], $info, 5), Io\Operation\SigWait::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, SIGUSR1));
attempt('sigwait timeout', fn () => @pcntl_sigtimedwait([SIGUSR1], $info, 0, 1000), Io\Operation\SigWait::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done, -1, 11));

attempt('getaddrinfo', fn () => gethostbyname('provider-invalid-results.invalid'), Io\Operation\GetAddrInfo::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done));
attempt('getnameinfo', fn () => gethostbyaddr('192.0.2.1'), Io\Operation\GetNameInfo::class,
    fn ($op) => $op->complete(Io\CompletionStatus::Done));

/* An operation still submitted to a queue completes from that queue */
$queue = new Io\Poll\OperationQueue();
attempt('submitted', fn () => usleep(1), Io\Operation\Timer::class, function ($op) use ($queue) {
    $queue->submit($op);
    return $op->complete(Io\CompletionStatus::Done);
});
var_dump($queue->countPending());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/provider-invalid-results.txt');
?>
--EXPECT--
read: Error: Io\Hooks\Hooks::run() cannot complete Io\Operation\Read with a result it did not produce, only a queue can
read eof: ''
write: Error: The IO provider completed an operation with an invalid result
write short: Error: The IO provider completed an operation with an invalid result
recv: Error: Io\Hooks\Hooks::run() cannot complete Io\Operation\Recv with a result it did not produce, only a queue can
accept: Error: Io\Hooks\Hooks::run() cannot complete Io\Operation\Accept with a result it did not produce, only a queue can
waitpid: Error: Io\Hooks\Hooks::run() cannot complete Io\Operation\WaitPid with a result it did not produce, only a queue can
bool(true)
int(3)
sigwait: Error: Io\Hooks\Hooks::run() cannot complete Io\Operation\SigWait with a result it did not produce, only a queue can
sigwait timeout: false
getaddrinfo: 'provider-invalid-results.invalid'
getnameinfo: '192.0.2.1'
submitted: Error: Io\Hooks\Hooks::run() must cancel the operation or wait for its completion from the queue it was submitted to
int(0)
