--TEST--
A failed seek keeps the stream position and the read buffer
--FILE--
<?php
class BaseStream {
    public $context;
    protected string $data = 'hello world';
    protected int $pos = 0;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool {
        return true;
    }

    public function stream_read(int $count): string {
        $ret = substr($this->data, $this->pos, $count);
        $this->pos += strlen($ret);
        return $ret;
    }

    public function stream_eof(): bool {
        return $this->pos >= strlen($this->data);
    }

    public function stream_seek(int $offset, int $whence): bool {
        $pos = match ($whence) {
            SEEK_SET => $offset,
            SEEK_CUR => $this->pos + $offset,
            SEEK_END => strlen($this->data) + $offset,
        };
        if ($pos < 0 || $pos > strlen($this->data)) {
            return false;
        }
        $this->pos = $pos;
        return true;
    }
}
class TestStream extends BaseStream {
    public function stream_tell(): int {
        return $this->pos;
    }
}
class StringTellStream extends BaseStream {
    public function stream_tell(): string {
        return 'unknown';
    }
}
class NoTellStream extends BaseStream {
}
stream_wrapper_register('test', TestStream::class);
stream_wrapper_register('stringtell', StringTellStream::class);
stream_wrapper_register('notell', NoTellStream::class);

$file = __DIR__ . '/stream_seek_failure_keeps_buffer.txt';
file_put_contents($file, 'hello world');

echo "-- plain file --\n";
$stream = fopen($file, 'r');
var_dump(fread($stream, 5));
var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), feof($stream));
var_dump(fseek($stream, -12, SEEK_END), ftell($stream), feof($stream));
var_dump(fread($stream, 10), feof($stream));
fclose($stream);

echo "-- plain file, write after failed seek --\n";
$stream = fopen($file, 'r+');
var_dump(fread($stream, 5));
var_dump(fseek($stream, -12, SEEK_END), fwrite($stream, '!'), ftell($stream));
fclose($stream);
var_dump(file_get_contents($file));

echo "-- user stream --\n";
$stream = fopen('test://', 'r');
var_dump(fread($stream, 5));
var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), feof($stream));
var_dump(fseek($stream, 12, SEEK_SET), ftell($stream), feof($stream));
var_dump(fseek($stream, 1, SEEK_END), ftell($stream), feof($stream));
var_dump(fread($stream, 10), feof($stream));
fclose($stream);

// stream_seek() moved the stream, so the buffered data of the old position
// is discarded although the seek failed
foreach (['stringtell' => 'stream_tell() returns no int', 'notell' => 'no stream_tell()'] as $wrapper => $label) {
    echo "-- user stream, $label --\n";
    $stream = fopen("$wrapper://", 'r');
    var_dump(fread($stream, 5));
    var_dump(fseek($stream, 0), ftell($stream), feof($stream));
    var_dump(fread($stream, 10), feof($stream));
    fclose($stream);
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stream_seek_failure_keeps_buffer.txt');
?>
--EXPECTF--
-- plain file --
string(5) "hello"
int(-1)
int(5)
bool(false)
int(-1)
int(5)
bool(false)
string(6) " world"
bool(true)
-- plain file, write after failed seek --
string(5) "hello"
int(-1)
int(1)
int(6)
string(11) "hello!world"
-- user stream --
string(5) "hello"
int(-1)
int(5)
bool(false)
int(-1)
int(5)
bool(false)
int(-1)
int(5)
bool(false)
string(6) " world"
bool(true)
-- user stream, stream_tell() returns no int --
string(5) "hello"
int(-1)
int(5)
bool(false)
string(10) "hello worl"
bool(false)
-- user stream, no stream_tell() --
string(5) "hello"

Warning: fseek(): NoTellStream::stream_tell is not implemented! in %s on line %d
int(-1)
int(5)
bool(false)
string(10) "hello worl"
bool(false)
