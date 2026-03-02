--TEST--
#[\Override]: Parent method is private.
--FILE--
<?php

class P {
    private function p(): void {}
}

class C extends P {
    #[\Override]
    public function p(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::p() has #[\Override] attribute, but no matching parent method exists in %s on line %d
