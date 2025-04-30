--TEST--
Validation for "Attribute" uses the class scope when evaluating constant ASTs
--FILE--
<?php
#[Attribute(parent::x)]
class x extends y {}

class y {
    protected const x = Attribute::TARGET_CLASS;
}

#[x]
class z {}

var_dump((new ReflectionClass(z::class))->getAttributes()[0]->newInstance());
?>
--EXPECT--
object(x)#1 (0) {
}
