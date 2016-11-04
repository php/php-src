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

--EXPECTF--
Fatal error: Call to undefined function _some_undefined_function() in %s%euser-stream-error.php on line %d
