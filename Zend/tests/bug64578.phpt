--TEST--
Bug #64578 (debug_backtrace in set_error_handler corrupts zend heap: segfault)
--FILE--
<?php

set_error_handler(function($no, $err) { var_dump($err); });

function x($s) { $s['a'] = 1; };
$y = '1';
x($y);
print_r($y);
--EXPECT--
string(25) "Illegal string offset 'a'"
1
