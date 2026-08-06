--TEST--
GH-21700: Append-mode open ignores a negative position returned by a user stream
--FILE--
<?php
class lyingstream {
    public $context;
    public function stream_open($path, $mode, $options, &$opened_path) { return true; }
    public function stream_seek($offset, $whence) { return true; }
    public function stream_tell(): int { return -42; }
    public function stream_eof() { return false; }
    public function stream_read($count) { return ''; }
}

stream_wrapper_register("test", "lyingstream");

// Opening in append mode triggers an internal SEEK_CUR to revise the initial
// position; if the user stream lies, ftell() must still report a non-negative
// value and a subsequent SEEK_CUR must not trip the position invariant.
$fp = fopen("test://foo", "ab");
var_dump(ftell($fp));
var_dump(fseek($fp, 0, SEEK_CUR));
fclose($fp);

stream_wrapper_unregister("test");
?>
--EXPECT--
int(0)
int(-1)
