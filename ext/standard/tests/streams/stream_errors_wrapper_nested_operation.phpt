--TEST--
Stream errors: wrapper errors after a nested stream operation are still reported
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

class ProxyStream
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
            $inner = fopen('error-stream://x', 'r', false, $GLOBALS['innerCtx']);
            fread($inner, 1);
            fclose($inner);
        }
        return $this->reads <= 2 ? str_repeat('B', $count + 1) : '';
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

stream_wrapper_register('error-stream', ErrorStream::class);
stream_wrapper_register('proxy', ProxyStream::class);

$innerCtx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_handler' => static function (array $errors): void {
        echo "inner handler: " . count($errors) . " error(s): {$errors[0]->code->name}\n";
    },
]]);

$outerCtx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_store' => StreamErrorStore::All,
    'error_handler' => static function (array $errors): void {
        echo "outer handler: " . count($errors) . " error(s)\n";
        foreach ($errors as $error) {
            echo "  {$error->code->name}: {$error->message}\n";
        }
    },
]]);

$stream = fopen('proxy://x', 'r', false, $outerCtx);
var_dump(strlen(stream_get_contents($stream)));
fclose($stream);
var_dump(count(stream_last_errors()));
?>
--EXPECT--
inner handler: 1 error(s): UserspaceInvalidReturn
outer handler: 2 error(s)
  UserspaceInvalidReturn: ProxyStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost
  UserspaceInvalidReturn: ProxyStream::stream_read - read 1 bytes more data than requested (8193 read, 8192 max) - excess data will be lost
int(16384)
int(2)
