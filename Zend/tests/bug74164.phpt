--TEST--
Bug #74164 (PHP hangs when an invalid value is dynamically passed to typehinted by-ref arg)
--FILE--
<?php

namespace Foo;

set_error_handler(function ($type, $msg) {
    throw new \Exception($msg);
});

call_user_func(function (array &$ref) {var_dump("xxx");}, 'not_an_array_variable');
?>
--EXPECTF--
Fatal error: Uncaught Exception: Foo\{closure:%s:%d}(): Argument #1 ($ref) must be passed by reference, value given in %s:%d
Stack trace:
#0 [internal function]: Foo\{closure:%s:%d}(2, '%s', '%s', 9)
#1 %sbug74164.php(%d): call_user_func(%s)
#2 {main}
  thrown in %sbug74164.php on line %d
