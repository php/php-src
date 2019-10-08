--TEST--
A "non well formed" notice converted to exception should result in a ZPP failure
--FILE--
<?php

set_error_handler(function($_, $msg) {
    throw new Exception($msg);
}, E_NOTICE);

try {
    wordwrap("foo", "123foo", "");
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Exception: A non well formed numeric value encountered in %s:%d
Stack trace:
#0 [internal function]: {closure}(%s)
#1 %s(%d): wordwrap('foo', '123foo', '')
#2 {main}
