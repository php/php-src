--TEST--
#[\Override]: Properties: On used trait without parent property.
--FILE--
<?php

trait T {
    #[\Override]
    public mixed $t;
}

class Foo {
    use T;
}

echo "Done";

?>
--EXPECTF--
Fatal error: Foo::$t has #[\Override] attribute, but no matching parent property exists in %s on line %d
