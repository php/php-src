--TEST--
Stream errors: operations of fibers suspended in a user wrapper keep their own errors
--FILE--
<?php
class SuspendingStream
{
    public $context;
    private int $reads = 0;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        $this->reads++;
        if ($this->reads === 2) {
            Fiber::suspend();
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

class StreamA extends SuspendingStream {}
class StreamB extends SuspendingStream {}

stream_wrapper_register('suspend-a', StreamA::class);
stream_wrapper_register('suspend-b', StreamB::class);

function read_all(string $name, string $url): void
{
    $context = stream_context_create(['stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_handler' => static function (array $errors) use ($name): void {
            foreach ($errors as $error) {
                echo "$name: ", strstr($error->message, ' - ', true), "\n";
            }
        },
    ]]);
    $stream = fopen($url, 'r', false, $context);
    var_dump(strlen(stream_get_contents($stream)));
}

$a = new Fiber(fn () => read_all('A', 'suspend-a://x'));
$b = new Fiber(fn () => read_all('B', 'suspend-b://x'));
$a->start();
$b->start();
$a->resume();
$b->resume();

$stream = fopen('suspend-a://x', 'r');
var_dump(strlen(fread($stream, 10)));
?>
--EXPECTF--
A: StreamA::stream_read
A: StreamA::stream_read
int(16384)
B: StreamB::stream_read
B: StreamB::stream_read
int(16384)

Warning: fread(): StreamA::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost in %s on line %d
int(10)
