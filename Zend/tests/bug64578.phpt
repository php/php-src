--TEST--
Bug #64578 (debug_backtrace in set_error_handler corrupts zend heap: segfault)
--FILE--
<?php

set_error_handler(function() { debug_backtrace(); });

function x($s) { $s['a'] = 1; };
$y = '1';
x($y);
print_r($y);
--EXPECTF--
1
