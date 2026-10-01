--TEST--
Stream errors: nested operation unwinding restores the parent operation
--FILE--
<?php
class ErrorStream
{
    public $context;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        return str_repeat('A', $count + 1);
    }

    public function stream_eof(): bool
    {
        return true;
    }

    public function stream_stat(): array
    {
        return [];
    }
}

class NestingStream
{
    public $context;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        $inner = fopen('php://memory', 'r');
        fread($inner, 1);
        fclose($inner);
        return 'x';
    }

    public function stream_eof(): bool
    {
        return true;
    }

    public function stream_stat(): array
    {
        return [];
    }
}

stream_wrapper_register('error-stream', ErrorStream::class);
stream_wrapper_register('nesting-stream', NestingStream::class);

$busy = false;
set_error_handler(static function (int $severity, string $message) use (&$busy): bool {
    echo "handler: $message\n";
    if ($busy) {
        return true;
    }
    $busy = true;
    $s = fopen('nesting-stream://x', 'r');
    fread($s, 1);
    fclose($s);
    $busy = false;
    return true;
});

$stream = fopen('error-stream://x', 'r');
var_dump(fread($stream, 1));
fclose($stream);
echo "done\n";
?>
--EXPECT--
handler: fread(): ErrorStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost
string(1) "A"
done
