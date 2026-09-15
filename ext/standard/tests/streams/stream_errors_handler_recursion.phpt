--TEST--
Stream errors: recursive error handler is stopped by the stack limit
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
?>
--INI--
zend.max_allowed_stack_size=512K
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
$ctx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_handler' => static function (array $errors) use (&$calls, &$ctx): void {
        $calls++;
        $stream = fopen('error-stream://x', 'r', false, $ctx);
        fread($stream, 1);
        fclose($stream);
    },
]]);

$stream = fopen('error-stream://x', 'r', false, $ctx);
try {
    fread($stream, 1);
} catch (Error $e) {
    echo get_class($e), ": ", $e->getMessage(), "\n";
}
fclose($stream);
var_dump($calls > 1);

$ctx = stream_context_create(['stream' => [
    'error_mode' => StreamErrorMode::Silent,
    'error_handler' => static function (array $errors): void {
        echo "handler: " . count($errors) . " error(s)\n";
    },
]]);
$stream = fopen('error-stream://x', 'r', false, $ctx);
var_dump(fread($stream, 1));
fclose($stream);
?>
--EXPECTF--
Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion?
bool(true)
handler: 1 error(s)
string(1) "A"
