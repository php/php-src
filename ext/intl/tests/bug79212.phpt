--TEST--
Bug #79212 (NumberFormatter::format() may detect wrong type)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not available');
if (!extension_loaded('gmp')) die('skip gmp extension not available');
?>
--FILE--
<?php
$fmt = new NumberFormatter('en_US', NumberFormatter::PATTERN_DECIMAL);
var_dump($fmt->format(gmp_init('823749273428379492374')));

$fmt = new NumberFormatter('en_US', NumberFormatter::PATTERN_DECIMAL);
var_dump($fmt->format([1], NumberFormatter::TYPE_INT64));
?>
--EXPECTF--
string(21) "823749273428379%c%c%c%c%c%c"

Fatal error: Uncaught TypeError: given value must not be an array in %s:%d
Stack trace:
#0 %s(%d): NumberFormatter->format(Array, 2)
#1 {main}
  thrown in %s on line %d
