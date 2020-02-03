--TEST--
using multiple access modifiers (attributes)
--FILE--
<?php

class test
{
    static public public public static final final $var;
}

echo "Done\n";

?>
--EXPECTF--
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
