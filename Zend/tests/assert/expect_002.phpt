--TEST--
test failing assertion
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
assert(false);
var_dump(true);
?>
--EXPECTF--
Fatal error: Uncaught exception 'AssertionException' with message 'assert(false)' in %sexpect_002.php:%d
Stack trace:
#0 %sexpect_002.php(%d): assert(false, 'assert(false)')
#1 {main}
  thrown in %sexpect_002.php on line %d
