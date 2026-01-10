--TEST--
#[\Override]: No parent class, but child implements matching interface (2).
--FILE--
<?php

interface I {
    public function i(): void;
}

class C extends P implements I {}

class P {
    #[\Override]
    public function i(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: P::i() has #[\Override] attribute, but no matching parent method exists in %s on line %d
