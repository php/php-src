--TEST--
IO hooks: stream errors of interleaved fibers reach the handler of their own stream
--FILE--
<?php
include __DIR__ . '/scheduler.inc';

class ProxyStream
{
    public $context;
    private $socket;
    private int $reads = 0;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        $this->socket = $GLOBALS['sockets'][parse_url($path, PHP_URL_HOST)];
        return true;
    }

    public function stream_read(int $count): string
    {
        $this->reads++;
        if ($this->reads === 2) {
            fread($this->socket, 16);
        }
        return $this->reads <= 2 ? str_repeat('x', $count + 1) : '';
    }

    public function stream_eof(): bool
    {
        return $this->reads >= 3;
    }

    public function stream_stat(): array
    {
        return [];
    }
}

class StreamA extends ProxyStream {}
class StreamB extends ProxyStream {}

stream_wrapper_register('proxy-a', StreamA::class);
stream_wrapper_register('proxy-b', StreamB::class);

function context(string $name): mixed
{
    return stream_context_create(['stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_handler' => static function (array $errors) use ($name): void {
            foreach ($errors as $error) {
                echo "$name: ", strstr($error->message, ' - ', true), "\n";
            }
        },
    ]]);
}

$scheduler = new Scheduler();
Io\Hooks\set_hooks($scheduler);

[$a, $feedA] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
[$b, $feedB] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, STREAM_IPPROTO_IP);
$sockets = ['a' => $a, 'b' => $b];

$scheduler->spawn(function () use ($feedB) {
    $stream = fopen('proxy-a://a', 'r', false, context('A'));
    stream_get_contents($stream);
    echo "A done\n";
    fwrite($feedB, 'b');
});
$scheduler->spawn(function () {
    $stream = fopen('proxy-b://b', 'r', false, context('B'));
    stream_get_contents($stream);
    echo "B done\n";
});
$scheduler->spawn(function () use ($feedA) {
    fwrite($feedA, 'a');
});
$scheduler->loop();
?>
--EXPECT--
A: StreamA::stream_read
A: StreamA::stream_read
A done
B: StreamB::stream_read
B: StreamB::stream_read
B done
