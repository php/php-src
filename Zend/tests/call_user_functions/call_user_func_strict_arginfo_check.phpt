--TEST--
Arginfo / zpp consistency check for call_user_func() in strict context
--FILE--
<?php
declare(strict_types=1);

namespace Foo;

// strlen() will be called with strict_types=0, so this is legal.
var_dump(call_user_func('strlen', false));

?>
--EXPECTF--
Fatal error: Uncaught TypeError: strlen(): Argument #1 ($string) must be of type string, false given in %s:%d
Stack trace:
#0 %s(%d): strlen(false)
#1 {main}
  thrown in %s on line %d
