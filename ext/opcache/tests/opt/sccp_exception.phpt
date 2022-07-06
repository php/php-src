--TEST--
Exception thrown during SCCP evaluation
--FILE--
<?php
var_dump(version_compare('1.2', '2.1', '??'));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: version_compare(): Argument #3 ($operator) must be a valid comparison operator in %s:%d
Stack trace:
#0 %s(%d): version_compare('1.2', '2.1', '??')
#1 {main}
  thrown in %s on line %d
