--TEST--
Readonly class may use readonly trait property with default value
--FILE--
<?php

trait TDefault {
    public readonly int $prop = 2;
}

readonly class C {
    use TDefault;
}

var_dump(new C()->prop);

?>
--EXPECT--
int(2)
