--TEST--
errmsg: multiple access type modifiers are not allowed (properties)
--FILE--
<?php

class test {
	public private $var;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Multiple access type modifiers are not allowed in %s on line %d
