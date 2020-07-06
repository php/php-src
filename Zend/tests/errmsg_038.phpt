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
Fatal error: Property test::$var cannot be declared final, the final modifier is allowed only for methods and classes in %s on line %d
