--TEST--
GH-23457 (imagebmp() writes to the stream one byte at a time)
--EXTENSIONS--
gd
--FILE--
<?php
class write_counter
{
    public $context;

    public static int $writes = 0;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool
    {
        return true;
    }

    public function stream_write(string $data): int
    {
        self::$writes++;
        return strlen($data);
    }

    public function stream_close(): void
    {
    }
}

stream_wrapper_register('gh23457', write_counter::class);

$im = imagecreatetruecolor(200, 200);
var_dump(imagebmp($im, 'gh23457://image.bmp'));
var_dump(write_counter::$writes < 100);
?>
--EXPECT--
bool(true)
bool(true)
