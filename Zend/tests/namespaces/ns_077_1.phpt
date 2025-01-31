--TEST--
077: Unknown compile-time constants in namespace
--FILE--
<?php
namespace foo;

function foo($a = array(0 => \unknown))
{
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "unknown" in %s:%d
Stack trace:
#0 %s(%d): foo\foo()
#1 {main}
  thrown in %sns_077_%d.php on line %d
