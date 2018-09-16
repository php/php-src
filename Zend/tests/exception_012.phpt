--TEST--
Test exception doesn't cause RSHUTDOWN bypass, variation 1
--INI--
assert.bail=1
assert.exception=1
--FILE--
<?php

$func = create_function('', 'define("Mommy", 1); assert(false);');
$func();

?>
--EXPECTHEADERS--
Content-type: text/html; charset=UTF-8
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d

Fatal error: Uncaught AssertionError: assert(false) in %sexception_012.php(%d) : runtime-created function:%d
Stack trace:
#0 %sexception_012.php(%d) : runtime-created function(%d): assert(false, 'assert(false)')
#1 %sexception_012.php(%d): __lambda_func()
#2 {main}
  thrown in %sexception_012.php(%d) : runtime-created function on line %d
