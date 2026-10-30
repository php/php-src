--TEST--
#[\Override]: Parent method is private (2).
--FILE--
<?php

class P {
    private function p(): void {}
}

class C extends P {
    #[\Override]
    private function p(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::p() has #[\Override] attribute, but no matching parent method exists in %s on line %d
