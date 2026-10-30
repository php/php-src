--TEST--
#[\Override]: Static method no parent class.
--FILE--
<?php

class C
{
    #[\Override]
    public static function c(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::c() has #[\Override] attribute, but no matching parent method exists in %s on line %d
