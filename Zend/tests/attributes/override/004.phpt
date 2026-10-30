--TEST--
#[\Override]: No parent class, but child implements matching interface.
--FILE--
<?php

interface I {
    public function i(): void;
}

class P {
    #[\Override]
    public function i(): void {}
}

class C extends P implements I {}

echo "Done";

?>
--EXPECTF--
Fatal error: P::i() has #[\Override] attribute, but no matching parent method exists in %s on line %d
