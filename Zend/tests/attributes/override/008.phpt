--TEST--
#[\Override]: On used trait without parent method.
--FILE--
<?php

trait T {
    #[\Override]
    public function t(): void {}
}

class Foo {
    use T;
}

echo "Done";

?>
--EXPECTF--
Fatal error: Foo::t() has #[\Override] attribute, but no matching parent method exists in %s on line %d
