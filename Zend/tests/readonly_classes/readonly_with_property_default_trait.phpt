--TEST--
Readonly class may use readonly trait property with default value
--FILE--
<?php

trait TDefault {
    public readonly int $prop = 2;
}

readonly class A {
    use TDefault;
}

var_dump(new A()->prop);

class B {
    use TDefault;
}

var_dump(new B()->prop);

?>
--EXPECT--
int(2)
int(2)
