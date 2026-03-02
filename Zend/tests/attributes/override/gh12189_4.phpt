--TEST--
#[Override] attribute in trait does not check for parent class implementations (Variant with __construct)
--FILE--
<?php

class A {
    public function __construct() {}
}

trait T {
    #[\Override]
    public function __construct() {
        echo 'foo';
    }
}

class D extends A {
    use T;
}
echo "Done";

?>
--EXPECTF--
Fatal error: D::__construct() has #[\Override] attribute, but no matching parent method exists in %s on line %d
