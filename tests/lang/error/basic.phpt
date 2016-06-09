--TEST--
Uncatched exceptions are properly stored.
--FILE--
<?php

new DateTime('1/1/11111');

?>
--EXPECTF--
Fatal error: Uncaught Exception: DateTime::__construct(): Failed to parse time string (1/1/11111) at position 8 (1): Unexpected character in %s
Stack trace:
#0 %s: DateTime->__construct('1/1/11111')
#1 {main}
  thrown in %s
