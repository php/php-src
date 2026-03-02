--TEST--
Exception thrown during SCCP evaluation, separate file variation
--FILE--
<?php
require __DIR__ . '/sccp_exception2.inc';
?>
--EXPECTF--
Fatal error: Uncaught ValueError: version_compare(): Argument #3 ($operator) must be a valid comparison operator in %s:%d
Stack trace:
#0 %s(%d): version_compare('1.2', '2.1', '??')
#1 %s(%d): require('%s')
#2 {main}
  thrown in %s on line %d
