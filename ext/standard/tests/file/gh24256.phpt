--TEST--
GH-24256 (fclose() returns false for stream wrappers without stream_flush())
--FILE--
<?php
class WrapperWithoutFlush {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    public function stream_write($data) {
        return strlen($data);
    }

    public function url_stat($path, $flags) {
        return false;
    }
}

class WrapperWithFailingFlush extends WrapperWithoutFlush {
    public function stream_flush() {
        return false;
    }
}

stream_wrapper_register('noflush', WrapperWithoutFlush::class);
stream_wrapper_register('failingflush', WrapperWithFailingFlush::class);

echo "Without stream_flush():\n";
$fp = fopen('noflush://foo', 'w');
var_dump(fwrite($fp, 'data'));
var_dump(fflush($fp));
var_dump(fwrite($fp, 'data'));
var_dump(fclose($fp));
var_dump(file_put_contents('noflush://foo', 'data'));
var_dump(copy(__FILE__, 'noflush://foo'));

echo "With a failing stream_flush():\n";
$fp = fopen('failingflush://foo', 'w');
var_dump(fwrite($fp, 'data'));
var_dump(fclose($fp));
var_dump(file_put_contents('failingflush://foo', 'data'));
?>
--EXPECT--
Without stream_flush():
int(4)
bool(false)
int(4)
bool(true)
int(4)
bool(true)
With a failing stream_flush():
int(4)
bool(false)
bool(false)
