--TEST--
#[\Override]: Properties: No parent class.
--FILE--
<?php

class C {
    #[\Override]
    public mixed $c;
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::$c has #[\Override] attribute, but no matching parent property exists in %s on line %d
