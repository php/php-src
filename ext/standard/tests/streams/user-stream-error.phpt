--TEST--
E_ERROR during UserStream Open
--FILE--
<?php

class FailStream {
  public function stream_open($path, $mode, $options, &$opened_path) {
    _some_undefined_function();
  }
}
stream_wrapper_register('mystream', 'FailStream');
fopen('mystream://foo', 'r');
echo 'Done';
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function _some_undefined_function() in %s%euser-stream-error.php:%d
Stack trace:
#0 [internal function]: FailStream->stream_open('mystream://foo', 'r', 0, NULL)
#1 %s%euser-stream-error.php(%d): fopen('mystream://foo', 'r')
#2 {main}
  thrown in %s%euser-stream-error.php on line %d
