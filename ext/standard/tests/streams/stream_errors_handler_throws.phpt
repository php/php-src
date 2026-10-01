--TEST--
Stream errors: exception thrown from the error handler
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

$ctx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_store' => StreamErrorStore::All,
    'error_handler' => static function (array $errors): void {
        throw new RuntimeException("handler: {$errors[0]->code->name}");
    },
]]);

$stream = fopen('error-stream://x', 'r', false, $ctx);
try {
    var_dump(fread($stream, 1));
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count(stream_last_errors()));
fclose($stream);

$stream = fopen('error-stream://x', 'r', false, $ctx);
try {
    var_dump(fread($stream, 1));
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
var_dump(count(stream_last_errors()));
fclose($stream);
?>
--EXPECT--
handler: UserspaceInvalidReturn
int(1)
handler: UserspaceInvalidReturn
int(1)
