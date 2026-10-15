--TEST--
GH-24256 (fclose() returns false for stream wrappers without stream_flush())
--FILE--
<?php
class TestStream {
    public $context;

    function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    function stream_write($data) {
        return strlen($data);
    }

    function url_stat($path, $flags) {
        return false;
    }
}

stream_wrapper_register('test', TestStream::class);

$fp = fopen('test://foo', 'w');
var_dump(fwrite($fp, 'data'));
var_dump(fclose($fp));

var_dump(file_put_contents('test://foo', 'data'));
var_dump(copy(__FILE__, 'test://foo'));
?>
--EXPECT--
int(4)
bool(true)
int(4)
bool(true)
