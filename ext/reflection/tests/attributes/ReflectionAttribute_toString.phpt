--TEST--
ReflectionAttribute::__toString
--FILE--
<?php

#[Foo, Bar(a: "foo", b: 1234), Baz("foo", 1234), X(NO_ERROR), Y(new stdClass)]
function foo() {}

$refl = new ReflectionFunction('foo');
echo $refl->getAttributes()[0];
echo $refl->getAttributes()[1];
echo $refl->getAttributes()[2];
echo $refl->getAttributes()[3];
echo $refl->getAttributes()[4];

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
Attribute [ X ] {
  - Arguments [1] {
    Argument #0 [ NO_ERROR ]
  }
}
Attribute [ Y ] {
  - Arguments [1] {
    Argument #0 [ new \stdClass() ]
  }
}
