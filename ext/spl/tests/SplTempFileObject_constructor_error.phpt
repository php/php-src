--TEST--
SPL SplTempFileObject constructor sets correct defaults when pass 0 arguments
--FILE--
<?php
new SplTempFileObject('invalid');
?>
--EXPECTF--
Fatal error: Uncaught exception 'RuntimeException' with message 'SplTempFileObject::__construct() expects parameter 1 to be long, string given' in %s
Stack trace:
#0 %s: SplTempFileObject->__construct('invalid')
#1 {main}
  thrown in %s
