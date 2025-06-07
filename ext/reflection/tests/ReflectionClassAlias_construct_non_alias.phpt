--TEST--
ReflectionClassAlias::__construct() - not an alias
--FILE--
<?php

$r = new ReflectionClassAlias( 'ReflectionClassAlias' );
?>
--EXPECTF--
Fatal error: Uncaught ReflectionException: "ReflectionClassAlias" is not an alias in %s:%d
Stack trace:
#0 %s(%d): ReflectionClassAlias->__construct('ReflectionClass...')
#1 {main}
  thrown in %s on line %d
