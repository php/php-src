--TEST--
Cannot use "decorated" with property group
--FILE--
<?php

class Test {
    public decorated Foo $bar, $baz;
}

?>
--EXPECTF--
Fatal error: Cannot declare multiple properties as "decorated" in %s on line %d
