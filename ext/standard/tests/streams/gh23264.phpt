--TEST--
GH-23264 (Streams: set_error_handler() callback frees the structured-handler context)
--FILE--
<?php
class OversizedReadStream
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
        return false;
    }

    public function stream_stat(): array
    {
        return [];
    }
}

stream_wrapper_register('oversized-read', OversizedReadStream::class);

$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Error,
        'error_handler' => static function (array $errors): void {
            echo "handler: {$errors[0]->code->name}\n";
        },
    ],
]);
$stream = fopen('oversized-read://input', 'r', false, $context);
unset($context);

set_error_handler(static function (int $severity, string $message) use (&$stream): bool {
    echo "legacy: $message\n";
    fclose($stream);
    return true;
});

var_dump(fread($stream, 1));
?>
--EXPECT--
legacy: fread(): OversizedReadStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost
handler: UserspaceInvalidReturn
string(1) "A"
