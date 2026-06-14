--TEST--
#[\Override]: Properties: No parent class, but child implements matching interface (2).
--FILE--
<?php

interface I {
    public mixed $i { get; }
}

class C extends P implements I {}

class P {
    #[\Override]
    public mixed $i;
}

echo "Done";

?>
--EXPECTF--
Fatal error: P::$i has #[\Override] attribute, but no matching parent property exists in %s on line %d
