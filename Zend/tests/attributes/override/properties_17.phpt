--TEST--
#[\Override]: Properties: Static property no parent class.
--FILE--
<?php

class C
{
    #[\Override]
    public static mixed $c;
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::$c has #[\Override] attribute, but no matching parent property exists in %s on line %d
