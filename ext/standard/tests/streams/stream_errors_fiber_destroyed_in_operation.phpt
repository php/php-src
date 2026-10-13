--TEST--
Stream errors: a fiber destroyed while suspended inside a stream operation
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
        if ($this->reads === 2 && Fiber::getCurrent()) {
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

stream_wrapper_register('suspend', SuspendingStream::class);

$context = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_store' => StreamErrorStore::None,
]]);

$fiber = new Fiber(function () use ($context) {
    $stream = fopen('suspend://x', 'r', false, $context);
    stream_get_contents($stream);
    echo "not reached\n";
});
$fiber->start();
unset($fiber);
gc_collect_cycles();
echo "destroyed\n";

$stream = fopen('suspend://x', 'r');
var_dump(strlen(stream_get_contents($stream)));
var_dump(count(stream_last_errors()));
?>
--EXPECTF--
destroyed

Warning: stream_get_contents(): SuspendingStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost in %s on line %d

Warning: stream_get_contents(): SuspendingStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost in %s on line %d
int(16384)
int(0)
