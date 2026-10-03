--TEST--
A failed seek keeps the stream position and the read buffer
--FILE--
<?php
class TestStream {
    public $context;
    private string $data = 'hello world';
    private int $pos = 0;

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

    public function stream_tell(): int {
        return $this->pos;
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
stream_wrapper_register('test', TestStream::class);

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
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/stream_seek_failure_keeps_buffer.txt');
?>
--EXPECT--
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
