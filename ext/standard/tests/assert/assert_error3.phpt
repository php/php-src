--TEST--
assert() - basic - Test recoverable error
--INI--
assert.active = 1
assert.warning = 1
assert.callback = f1
assert.quiet_eval = 0
assert.bail = 0
error_reporting = -1
display_errors = 1
--FILE--
<?php
var_dump($r2 = assert("0 $ 0"));
--EXPECTF--

Parse error: syntax error, unexpected '$', expecting ';' in %s(2) : assert code on line 1

Catchable fatal error: assert(): Failure evaluating code: 
0 $ 0 in %s on line 2

