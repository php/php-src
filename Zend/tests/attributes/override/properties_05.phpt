--TEST--
#[\Override]: Properties: No parent interface.
--FILE--
<?php

interface I {
    #[\Override]
    public mixed $i { get; }
}

interface II extends I {}


class C implements II {
    public mixed $i;
}

class C2 implements I {
    public mixed $i;
}

echo "Done";

?>
--EXPECTF--
Fatal error: I::$i has #[\Override] attribute, but no matching parent property exists in %s on line %d
