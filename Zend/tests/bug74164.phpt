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
Fatal error: Uncaught Exception: {closure:%s:%d}(): Argument #1 ($ref) must be passed by reference, value given in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(2, '%s', '%s', 9)
#1 {main}
  thrown in %sbug74164.php on line %d
