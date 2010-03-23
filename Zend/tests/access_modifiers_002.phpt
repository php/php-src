--TEST--
using multiple access modifiers (attributes)
--FILE--
<?php

class test {
	static public public static final public final $var;
}

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
