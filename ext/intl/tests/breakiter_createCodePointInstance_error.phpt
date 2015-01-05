--TEST--
IntlBreakIterator::createCodePointInstance(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(IntlBreakIterator::createCodePointInstance(array()));
--EXPECTF--

Warning: IntlBreakIterator::createCodePointInstance() expects exactly 0 parameters, 1 given in %s on line %d

Warning: IntlBreakIterator::createCodePointInstance(): breakiter_create_code_point_instance: bad arguments in %s on line %d
NULL

