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
?>
--EXPECTF--
Fatal error: Uncaught Error: Call to private IntlBreakIterator::__construct() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
