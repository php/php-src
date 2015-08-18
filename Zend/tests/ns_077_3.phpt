--TEST--
077: Unknown compile-time constants in namespace
--FILE--
<?php
namespace foo;

function foo($a = array(namespace\unknown => unknown))
{
}

foo();
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant 'foo\unknown' in %sns_077_%d.php:%d
Stack trace:
#0 %s(%d): foo\foo()
#1 {main}
  thrown in %sns_077_%d.php on line %d
