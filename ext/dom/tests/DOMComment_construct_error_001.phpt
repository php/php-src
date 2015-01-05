--TEST--
DOMComment::__construct() with more arguments than acceptable.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$comment = new DOMComment("comment1", "comment2");
?>
--EXPECTF--
Fatal error: Uncaught exception 'DOMException' with message 'DOMComment::__construct() expects at most 1 parameter, 2 given' in %s:%d
Stack trace:
#0 %s(%d): DOMComment->__construct('comment1', 'comment2')
#1 {main}
  thrown in %s on line %d