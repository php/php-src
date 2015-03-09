--TEST--
Throw reference
--FILE--
<?php

$e = new Exception;
$ref =& $e;
throw $e;

?>
--EXPECTF--
Fatal error: Uncaught exception 'Exception' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
