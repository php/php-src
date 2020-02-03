--TEST--
using multiple access modifiers (methods)
--FILE--
<?php

class test
{
    final public static public static public final function foo()
    {
    }
}

echo "Done\n";

?>
--EXPECTF--
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
