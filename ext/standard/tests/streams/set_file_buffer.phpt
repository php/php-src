--TEST--
int set_file_buffer ( resource $stream , int $buffer );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
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
var_dump(set_file_buffer($fd, 50));
var_dump(stream_set_chunk_size($fd, 42));
var_dump(fwrite($fd, str_repeat('0', 70)));
?>
--CLEAN--
<?php
fclose($fd);
unset($fd);
?>
--EXPECTF--
bool(true)
option: %d, %d, %d
int(%i)
int(%d)
size: %d
size: %d
int(%d)
