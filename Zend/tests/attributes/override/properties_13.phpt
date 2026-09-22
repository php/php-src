--TEST--
#[\Override]: Properties: Redeclared trait property.
--FILE--
<?php

trait T {
    public mixed $t;
}

class C {
    use T;

    #[\Override]
    public mixed $t;
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::$t has #[\Override] attribute, but no matching parent property exists in %s on line %d
