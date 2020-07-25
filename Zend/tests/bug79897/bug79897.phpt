--TEST--
bug79897: Promoted constructor params with attribs cause crash
--FILE--
<?php

@@Attribute
class B {
    public function __construct($value)
    {
    }
}

class A {
    public function __construct(
        @@B(12) public $b
    )
    {
    }
}

echo "file compiled successfully";
--EXPECT--
file compiled successfully
