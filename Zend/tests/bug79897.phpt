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

var_dump((new ReflectionParameter(['A', '__construct'], 'b'))->getAttributes()[0]->getArguments());
var_dump((new ReflectionProperty('A', 'b'))->getAttributes()[0]->getArguments());
?>
--EXPECT--
array(1) {
  [0]=>
  int(12)
}
array(1) {
  [0]=>
  int(12)
}
