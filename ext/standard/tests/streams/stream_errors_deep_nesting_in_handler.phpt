--TEST--
Stream errors: deeply nested operations started from the error handler
--FILE--
<?php
const OUTER_DEPTH = 9;
const HANDLER_DEPTH = 8;

class DeepStream
{
    public $context;
    public static int $left = 0;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        if (--self::$left > 0) {
            $f = fopen('deep-stream://x', 'r', false, $GLOBALS['ctx']);
            $s = fread($f, 1);
            fclose($f);
            return $s;
        }
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

class WideStream
{
    public $context;
    public static int $left = 0;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_read(int $count): string
    {
        if (--self::$left > 0) {
            $f = fopen('wide-stream://x', 'r');
            $s = fread($f, 1);
            fclose($f);
            return $s;
        }
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

stream_wrapper_register('deep-stream', DeepStream::class);
stream_wrapper_register('wide-stream', WideStream::class);

$busy = false;
$ctx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_store' => StreamErrorStore::All,
    'error_handler' => static function (array $errors) use (&$busy): void {
        echo "handler: {$errors[0]->code->name}\n";
        if ($busy) {
            return;
        }
        $busy = true;
        WideStream::$left = HANDLER_DEPTH;
        $f = fopen('wide-stream://x', 'r');
        fread($f, 1);
        fclose($f);
        $busy = false;
    },
]]);

DeepStream::$left = OUTER_DEPTH;
$stream = fopen('deep-stream://x', 'r', false, $ctx);
var_dump(fread($stream, 1));
fclose($stream);
var_dump(count(stream_last_errors()));
echo "done\n";
?>
--EXPECT--
handler: UserspaceInvalidReturn
string(1) "A"
int(1)
done
