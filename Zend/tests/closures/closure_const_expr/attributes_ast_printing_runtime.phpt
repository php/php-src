--TEST--
AST printing for closures in attributes at runtime
--FILE--
<?php

#[Attr(static function ($foo) {
    echo $foo;
})]
function foo() { }

$r = new ReflectionFunction('foo');
foreach ($r->getAttributes() as $attribute) {
    echo $attribute;
}

?>
--EXPECT--
Attribute [ Attr ] {
  - Arguments [1] {
    Argument #0 [ Closure({closure:foo():3}) ]
  }
}
