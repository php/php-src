--TEST--
IntlBreakIterator::__construct() should not be callable
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

new IntlBreakIterator();
--EXPECTF--

Fatal error: Call to private IntlBreakIterator::__construct() from invalid context in %s on line %d
