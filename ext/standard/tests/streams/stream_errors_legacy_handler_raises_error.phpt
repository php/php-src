--TEST--
Stream errors: stream error raised from a legacy error handler while reporting
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

stream_wrapper_register('error-stream', ErrorStream::class);

$calls = 0;
set_error_handler(static function (int $severity, string $message) use (&$calls): bool {
    $calls++;
    echo "handler: $message\n";
    include __DIR__ . '/stream_errors_legacy_handler_raises_error_missing.inc';
    echo "handler end\n";
    return true;
});

$stream = fopen('error-stream://x', 'r');
var_dump(fread($stream, 1));
fclose($stream);
var_dump($calls);
?>
--EXPECTF--
handler: fread(): ErrorStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost

Warning: include(): Failed to open stream: No such file or directory in %s on line %d

Warning: include(): Failed opening '%sstream_errors_legacy_handler_raises_error_missing.inc' for inclusion (include_path='%s') in %s on line %d
handler end
string(1) "A"
int(1)
