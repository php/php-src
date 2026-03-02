--TEST--
stream_select allows null for microsecond timeout if timeout is null
--FILE--
<?php

$read = [fopen(__FILE__, 'r')];
$write = null;
$except = null;

error_reporting(-1);
set_error_handler(function ($errno, $errstr) {
    print $errno . " " . $errstr . "\n";
});

stream_select($read, $write, $except, null, null);
var_dump($read);

print "\n";

stream_select($read, $write, $except, null, 0);

stream_select($read, $write, $except, null, 1);
?>
--EXPECTF--
array(1) {
  [0]=>
  resource(%d) of type (stream)
}


Fatal error: Uncaught ValueError: stream_select(): Argument #5 ($microseconds) must be null when argument #4 ($seconds) is null in %s
Stack trace:
#0 %s stream_select(Array, NULL, NULL, NULL, 1)
#1 {main}
%s
