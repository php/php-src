--TEST--
stream_is_local() on streams opened through compress.bzip2://
--EXTENSIONS--
bz2
--FILE--
<?php

class RemoteWrapper {
    public $context;
    private $fp;
    function stream_open($path, $mode, $options, &$opened_path) {
        $this->fp = fopen(substr($path, strlen('remote://')), $mode);
        return $this->fp !== false;
    }
    function stream_read($count) { return fread($this->fp, $count); }
    function stream_eof() { return feof($this->fp); }
    function stream_close() { fclose($this->fp); }
    function stream_cast($as) { return $this->fp; }
}
stream_wrapper_register('remote', 'RemoteWrapper', STREAM_IS_URL);

$file = __DIR__ . '/stream_is_local_nested.bz2';
file_put_contents($file, bzcompress('hello'));

$fp = fopen('compress.bzip2://' . $file, 'r');
var_dump(stream_is_local($fp));
var_dump(fread($fp, 10));
fclose($fp);

$fp = fopen('compress.bzip2://remote://' . $file, 'r');
var_dump(stream_is_local($fp));
var_dump(fread($fp, 10));
fclose($fp);

?>
--CLEAN--
<?php
unlink(__DIR__ . '/stream_is_local_nested.bz2');
?>
--EXPECT--
bool(true)
string(5) "hello"
bool(false)
string(5) "hello"
