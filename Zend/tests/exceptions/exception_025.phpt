--TEST--
zend.exception_string_param_max_len ini setting
--FILE--
<?php

function main($arg) {
    echo (new Exception()), "\n";
}
var_dump(ini_set('zend.exception_string_param_max_len', '-1'));
var_dump(ini_set('zend.exception_string_param_max_len', '1000001'));
var_dump(ini_set('zend.exception_string_param_max_len', '1000000'));
var_dump(ini_set('zend.exception_string_param_max_len', '20'));
main('short');
main('123456789012345678901234567890');
var_dump(ini_set('zend.exception_string_param_max_len', '0'));
main('short');
main('');

?>
--EXPECTF--
bool(false)
bool(false)
string(2) "15"
string(7) "1000000"
Exception in %s:%d
Stack trace:
#0 %s(10): main('short')
#1 {main}
Exception in %s:%d
Stack trace:
#0 %s(11): main('12345678901234567890...')
#1 {main}
string(2) "20"
Exception in %s:%d
Stack trace:
#0 %s(13): main('...')
#1 {main}
Exception in %s:%d
Stack trace:
#0 %s(14): main('')
#1 {main}
