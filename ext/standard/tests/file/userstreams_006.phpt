--TEST--
User-space streams: set_options returns "not implemented" for unhandled option types
--FILE--
<?php
class test_wrapper {
    function stream_open($path, $mode, $openedpath) {
        return true;
    }
    function stream_eof() {
        return false;
    }
    function stream_write($data) {
        echo "size: ", strlen($data), "\n";
        return strlen($data);
    }
    function stream_set_option($option, $arg1, $arg2) {
        echo "option: ", $option, ", ", $arg1, ", ", $arg2, "\n";
        return false;
    }
}

var_dump(stream_wrapper_register('test', 'test_wrapper'));

$fd = fopen("test://foo","r");

var_dump(stream_set_write_buffer($fd, 50));
var_dump(stream_set_chunk_size($fd, 42));

var_dump(fwrite($fd, str_repeat('0', 70)));
?>
--EXPECT--
bool(true)
option: 3, 2, 50
int(-1)
int(8192)
size: 70
int(70)
