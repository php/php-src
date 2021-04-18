--TEST--
stream_select allows null for microsecond timeout if timeout is null
--FILE--
<?php

$read = [fopen(__FILE__, 'r')];
$write = null;
$except = null;

error_reporting(-1);
set_error_handler(function ($errno, $errstr) {
    throw new \Exception($errstr);
});

stream_select($read, $write, $except, null, null);
var_dump($read);

stream_select($read, $write, $except, null, 0);
?>
--EXPECTF--
array(1) {
  [0]=>
  resource(%d) of type (stream)
}

Fatal error: Uncaught ValueError: stream_select(): Argument #4 ($seconds) must not be null if argument #5 ($microseconds) is specified and non-null in %s
Stack trace:
#0 %s stream_select(Array, NULL, NULL, NULL, 0)
#1 {main}
%s
