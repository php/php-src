--TEST--
#[\Override]: Properties: Parent property is private, child property is private.
--FILE--
<?php

class P {
    private mixed $p;
}

class C extends P {
    #[\Override]
    private mixed $p;
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::$p has #[\Override] attribute, but no matching parent property exists in %s on line %d
