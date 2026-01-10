--TEST--
#[\Override]: Redeclared trait method.
--FILE--
<?php

trait T {
    public function t(): void {}
}

class C {
    use T;

    #[\Override]
    public function t(): void {}
}

echo "Done";

?>
--EXPECTF--
Fatal error: C::t() has #[\Override] attribute, but no matching parent method exists in %s on line %d
