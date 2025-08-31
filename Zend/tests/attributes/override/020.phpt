--TEST--
#[\Override]: Invalid anonymous class.
--FILE--
<?php

interface I {
    public function i(): void;
}

new class () implements I {
    public function i(): void {}

    #[\Override]
    public function c(): void {}
};

echo "Done";

?>
--EXPECTF--
Fatal error: I@anonymous::c() has #[\Override] attribute, but no matching parent method exists in %s on line %d
