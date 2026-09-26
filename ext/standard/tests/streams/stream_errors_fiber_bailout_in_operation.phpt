--TEST--
Stream errors: a fiber bailing out inside a stream operation leaves the main flow clean
--FILE--
<?php
class FatalStream
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
            eval('function strlen() {}');
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

stream_wrapper_register('fatal', FatalStream::class);

register_shutdown_function(function () {
    include __DIR__ . '/stream_errors_fiber_bailout_in_operation_missing.inc';
    $stream = fopen('fatal://x', 'r');
    var_dump(strlen(fread($stream, 10)));
});

$fiber = new Fiber(function () {
    $stream = fopen('fatal://x', 'r');
    stream_get_contents($stream);
});
$fiber->start();
?>
--EXPECTF--
Fatal error: Cannot redeclare function strlen() in %s : eval()'d code on line %d

Warning: include(): Failed to open stream: No such file or directory in %s on line %d

Warning: include(): Failed opening '%s' for inclusion (include_path=%s) in %s on line %d

Warning: fread(): FatalStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost in %s on line %d
int(10)
