--TEST--
Test exception doesn't cause RSHUTDOWN bypass, variation 0
--INI--
zend.assertions=1
--FILE--
<?php

define ("XXXXX", 1);
assert(false);

?>
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Fatal error: Uncaught AssertionError: assert(false) in %sexception_011.php:%d
Stack trace:
#0 %sexception_011.php(%d): assert(false, 'assert(false)')
#1 {main}
  thrown in %sexception_011.php on line %d
