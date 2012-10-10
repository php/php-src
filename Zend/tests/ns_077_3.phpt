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
Fatal error: Undefined constant 'foo\unknown' in %sns_077_%d.php on line %d
