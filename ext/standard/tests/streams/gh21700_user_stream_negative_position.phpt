--TEST--
GH-21700: User stream returning negative position must not trigger an assertion
--FILE--
<?php
class mystream {
    public int $pos = 0;
    public function stream_open($path, $mode, $options, &$opened_path) { return true; }
    public function stream_seek($offset, $whence) {
        // Blindly store the offset, mimicking a buggy user implementation.
        $this->pos = $offset;
        return true;
    }
    public function stream_tell() { return $this->pos; }
    public function stream_eof() { return false; }
    public function stream_read($count) { return ''; }
}

class lyingstream extends mystream {
    public function stream_tell(): int { return -42; }
}

// 1. SEEK_CUR with a negative resulting absolute position is rejected
//    before invoking the user seek hook, so stream->position stays valid.
stream_wrapper_register("test", "mystream");
$fp = fopen("test://foo", "rb");
var_dump(fseek($fp, -1, SEEK_CUR));
var_dump(ftell($fp));
var_dump(fseek($fp, 0, SEEK_CUR));
fclose($fp);
stream_wrapper_unregister("test");

// 2. A user stream_tell() returning a negative position is rejected,
//    keeping stream->position non-negative.
stream_wrapper_register("test", "lyingstream");
$fp = fopen("test://foo", "rb");
var_dump(fseek($fp, 100, SEEK_SET));
var_dump(ftell($fp));
var_dump(fseek($fp, 0, SEEK_CUR));
fclose($fp);
stream_wrapper_unregister("test");
?>
--EXPECT--
int(-1)
int(0)
int(0)
int(-1)
int(0)
int(-1)
