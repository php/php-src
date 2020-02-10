--TEST--
Cannot use "decorated" on multiple properties
--FILE--
<?php

class Test {
    public decorated Foo $foo;
    public decorated Bar $bar;
}

?>
--EXPECTF--
Fatal error: Cannot declare multiple properties as "decorated" in %s on line %d
