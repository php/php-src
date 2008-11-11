--TEST--
077: Unknown compile-time constants in namespace
--FILE--
<?php

function foo($a = array(0 => \unknown))
{
}

foo();
--EXPECTF--
Fatal error: Undefined constant 'unknown' in %sns_077_%d.php on line %d
