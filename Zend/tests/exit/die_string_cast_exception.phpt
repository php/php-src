--TEST--
Bug #79777: String cast exception during die should be handled gracefully
--FILE--
<?php

die(new stdClass);

?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class stdClass could not be converted to string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
