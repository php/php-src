--TEST--
ReflectionClassAlias::__construct() - missing class
--FILE--
<?php

$r = new ReflectionClassAlias( 'missing' );
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: Class "missing" does not exist in %s:%d
Stack trace:
#0 %s(%d): ReflectionClassAlias->__construct('missing')
#1 {main}
  thrown in %s on line %d
