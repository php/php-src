--TEST--
Test using an Exception as the previous Throwable for an Error
--CREDITS--
Aaron Piotrowski <aaron@icicle.io>
--FILE--
<?php
throw new Error('Error message', 0, new Exception('Exception message'));
?>
--EXPECTF--

Fatal error: Uncaught Exception: Exception message in %s:%d
Stack trace:
#0 {main}

Next Error: Error message in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
