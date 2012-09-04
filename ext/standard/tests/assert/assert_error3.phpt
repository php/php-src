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
$sa = "0 $ 0";
var_dump($r2 = assert($sa));
--EXPECTF--

Parse error: syntax error, unexpected '$' in %s(3) : assert code on line 1

Catchable fatal error: assert(): Failure evaluating code: 
0 $ 0 in %s on line 3

