--TEST--
errmsg: properties cannot be final
--FILE--
<?php

class test {
	final $var = 1;
}

echo "Done\n";
?>
--EXPECTF--
Fatal error: Cannot declare property test::$var final, the final modifier is allowed only for methods and classes in %s on line %d
