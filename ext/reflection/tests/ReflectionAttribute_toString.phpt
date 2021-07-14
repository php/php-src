--TEST--
ReflectionAttribute::__toString
--FILE--
<?php

#[Foo, Bar(a: "foo", b: 1234), Baz("foo", 1234), XXX(ERROR)]
function foo() {}

$refl = new ReflectionFunction('foo');
echo $refl->getAttributes()[0];
echo $refl->getAttributes()[1];
echo $refl->getAttributes()[2];
try {
    echo $refl->getAttributes()[3];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Attribute [ Foo ]
Attribute [ Bar ] {
  - Arguments [2] {
    Argument #0 [ a = 'foo' ]
    Argument #1 [ b = 1234 ]
  }
}
Attribute [ Baz ] {
  - Arguments [2] {
    Argument #0 [ 'foo' ]
    Argument #1 [ 1234 ]
  }
}
Undefined constant "ERROR"
