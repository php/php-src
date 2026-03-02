--TEST--
#[\Override]: No parent class.
--FILE--
<?php

class C
{
    #[\Override]
    public function c(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::c() has #[\Override] attribute, but no matching parent method exists in %s on line %d
