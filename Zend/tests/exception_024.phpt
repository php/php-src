--TEST--
zend.exception_string_param_max_len ini setting
--INI--
zend.exception_string_param_max_len = 23
--FILE--
<?php

function main($arg) {
    throw new Exception();
}
main('123456789012345678901234567890');

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): main('12345678901234567890123...')
#1 {main}
  thrown in %s on line %d
