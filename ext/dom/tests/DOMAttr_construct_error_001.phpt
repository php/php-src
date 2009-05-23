--TEST--
DOMAttr __construct() with no arguments.
--CREDITS--
Josh Sweeney <jsweeney@alt-invest.net>
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$attr = new DOMAttr();
?>
--EXPECTF--
Fatal error: Uncaught exception 'DOMException' with message 'DOMAttr::__construct() expects at least 1 parameter, 0 given' in %s:%d
Stack trace:
#0 %s(%d): DOMAttr->__construct()
#1 {main}
  thrown in %s on line %d
