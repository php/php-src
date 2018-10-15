--TEST--
Test using an Error as the previous Throwable for an Exception
--CREDITS--
Aaron Piotrowski <aaron@icicle.io>
--FILE--
<?php
throw new Exception('Exception message', 0, new Error('Error message'));
?>
--EXPECTF--
Fatal error: Uncaught Error: Error message in %s:%d
Stack trace:
#0 {main}

Next Exception: Exception message in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
