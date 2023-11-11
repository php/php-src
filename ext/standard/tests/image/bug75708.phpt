--TEST--
Bug #75708 (getimagesize with "&$imageinfo" fails on StreamWrappers)
--FILE--
<?php

class FSStreamWrapper {
    public $context;

    public $handle;

    function stream_open($file, $mode) {
        $this->handle = fopen(str_replace('fs://', __DIR__ . '/', $file), $mode);
        return true;
    }
    function stream_read($count) {
        return fread($this->handle, $count);
    }
    function stream_eof() {
        return feof($this->handle);
    }
    function stream_seek($offset, $whence) {
        return fseek($this->handle, $offset, $whence) === 0;
    }
    function stream_stat() {
        return fstat($this->handle);
    }
    function url_stat($file) {
        return stat(str_replace('fs://', '', $file));
    }
    function stream_tell() {
        return ftell($this->handle);
    }
    function stream_close() {
        fclose($this->handle);
    }
}

stream_register_wrapper('fs', 'FSStreamWrapper');

var_dump(getimagesize('fs://bug75708.jpg', $info));

?>
--EXPECT--
array(7) {
  [0]=>
  int(10)
  [1]=>
  int(10)
  [2]=>
  int(2)
  [3]=>
  string(22) "width="10" height="10""
  ["bits"]=>
  int(8)
  ["channels"]=>
  int(3)
  ["mime"]=>
  string(10) "image/jpeg"
}

