--TEST--
Bug #71287 (Error message contains hexadecimal instead of decimal number)
--FILE--
<?php
class Stream {
    public $context;

    public function stream_open($path, $mode, $options, $opened_path) {
        return true;
    }

    public function stream_write($data) {
        return strlen($data) - 2;
    }
}

stream_wrapper_register('test', Stream::class);
file_put_contents('test://file.txt', 'foobarbaz');
?>
--EXPECTF--
Warning: file_put_contents(): Only 7 of 9 bytes written, possibly out of free disk space in %sbug71287.php on line %d
