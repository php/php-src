--TEST--
DOMDocumentFragment::__construct() with too many errors.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fragment = new DOMDocumentFragment("root");
?>
--EXPECTF--
Fatal error: Uncaught exception 'DOMException' with message 'DOMDocumentFragment::__construct() expects exactly 0 parameters, 1 given' in %s:%d
Stack trace:
#0 %s(%d): DOMDocumentFragment->__construct('root')
#1 {main}
  thrown in %s on line %d