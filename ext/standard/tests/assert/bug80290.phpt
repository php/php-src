--TEST--
Bug #80290: Double free when ASSERT_CALLBACK is used with a dynamic message
--FILE--
<?php

assert_options(ASSERT_CALLBACK, function($file, $line, $unused, $message) {
    var_dump($message);
});

$x = 'x';
assert(false, 'Dynamic message: ' . $x);

?>
--EXPECTF--
string(18) "Dynamic message: x"

Fatal error: Uncaught AssertionError: Dynamic message: x in %s:%d
Stack trace:
#0 %s(%d): assert(false, 'Dynamic message...')
#1 {main}
  thrown in %s on line %d
