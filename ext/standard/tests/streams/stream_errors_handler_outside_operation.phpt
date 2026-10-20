--TEST--
Stream errors: errors raised inside a handler are reported outside the current operation
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

class OuterStream
{
    public $context;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        $inner = fopen('error-stream://x', 'r', false, $GLOBALS['ctx']);
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
stream_wrapper_register('outer-stream', OuterStream::class);

$ctx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_handler' => static function (array $errors): void {
        echo "handler start: {$errors[0]->code->name}\n";
        include __DIR__ . '/stream_errors_handler_outside_operation_missing.inc';
        echo "handler end\n";
    },
]]);

$stream = fopen('outer-stream://x', 'r');
var_dump(fread($stream, 1));
fclose($stream);
var_dump(count(stream_last_errors()));
echo "done\n";
?>
--EXPECTF--
handler start: UserspaceInvalidReturn

Warning: include(): Failed to open stream: No such file or directory in %s on line %d

Warning: include(): Failed opening '%sstream_errors_handler_outside_operation_missing.inc' for inclusion (include_path='%s') in %s on line %d
handler end
string(1) "x"
int(1)
done
