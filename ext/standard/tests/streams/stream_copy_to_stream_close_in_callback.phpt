--TEST--
stream_copy_to_stream() when a user wrapper closes the other stream
--FILE--
<?php
class Wrapper {
    public $context;
    private int $left = 3;

    public function stream_open($path, $mode, $options, &$opened_path): bool {
        return true;
    }

    public function stream_read(int $count): string|false {
        self::closeOther();

        return $this->left-- > 0 ? 'abc' : '';
    }

    public function stream_write(string $data): int {
        self::closeOther();

        return strlen($data);
    }

    public function stream_eof(): bool {
        return $this->left <= 0;
    }

    private static function closeOther(): void {
        if (isset($GLOBALS['close'])) {
            $stream = $GLOBALS['close'];
            unset($GLOBALS['close']);
            var_dump(fclose($stream));
        }
    }
}

stream_wrapper_register('test', 'Wrapper');

echo "The destination closes the source:\n";
$src = fopen('php://memory', 'w+');
fwrite($src, str_repeat('x', 100000));
rewind($src);
$GLOBALS['close'] = $src;
var_dump(stream_copy_to_stream($src, fopen('test://', 'w')));
var_dump(fclose($src));

echo "The source closes the destination:\n";
$dest = fopen('php://memory', 'w+');
$GLOBALS['close'] = $dest;
var_dump(stream_copy_to_stream(fopen('test://', 'r'), $dest));
var_dump(fclose($dest));
?>
--EXPECTF--
The destination closes the source:

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
int(100000)
bool(true)
The source closes the destination:

Warning: fclose(): %d is not a valid stream resource in %s on line %d
bool(false)
int(9)
bool(true)
