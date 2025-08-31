--TEST--
#[\Override]: Properties: No parent class, but child implements matching interface.
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

class P {
    #[\Override]
    public mixed $i;
}

class C extends P implements I {}

echo "Done";

?>
--EXPECTF--
Fatal error: P::$i has #[\Override] attribute, but no matching parent property exists in %s on line %d
