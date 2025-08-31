--TEST--
Exception thrown during SCCP evaluation, strict types variation
--FILE--
<?php

declare(strict_types=1);
var_dump(str_contains("123", 1));

?>
--EXPECTF--
Fatal error: Uncaught TypeError: str_contains(): Argument #2 ($needle) must be of type string, int given in %s:%d
Stack trace:
#0 %s(%d): str_contains('123', 1)
#1 {main}
  thrown in %s on line %d
