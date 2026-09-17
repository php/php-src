--TEST--
GH-23262 (stream_get_meta_data() uses the stream after error_handler closes it)
--FILE--
<?php
class InvalidEofStream
{
    public $context;

    public function stream_open($path, $mode, $options, &$openedPath): bool
    {
        return true;
    }

    public function stream_eof()
    {
        return [];
    }

    public function stream_stat(): array
    {
        return [];
    }
}

stream_wrapper_register('invalid-eof', InvalidEofStream::class);

$stream = null;
$context = stream_context_create([
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_handler' => static function (array $errors) use (&$stream): void {
            echo "handler: {$errors[0]->code->name}\n";
            fclose($stream);
        },
    ],
]);

$stream = fopen('invalid-eof://input', 'r', false, $context);
$meta = stream_get_meta_data($stream);
var_dump($meta['eof'], $meta['wrapper_type'], $meta['uri']);
?>
--EXPECT--
handler: UserspaceInvalidReturn
bool(true)
string(10) "user-space"
string(19) "invalid-eof://input"
