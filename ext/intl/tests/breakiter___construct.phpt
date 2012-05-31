--TEST--
BreakIterator::__construct() should not be callable
--SKIPIF--
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

new BreakIterator();
--EXPECTF--

Fatal error: Call to private BreakIterator::__construct() from invalid context in %s on line %d
