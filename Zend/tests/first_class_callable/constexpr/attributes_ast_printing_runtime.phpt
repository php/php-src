--TEST--
AST printing for FCC in attributes at runtime
--FILE--
<?php

namespace Test;

class Clazz {
	#[Attr(strrev(...), \strrev(...), Clazz::foo(...), self::foo(...))]
	function foo() { }
}

$r = new \ReflectionMethod(Clazz::class, 'foo');
foreach ($r->getAttributes() as $attribute) {
    echo $attribute;
}

?>
--EXPECT--
Attribute [ Test\Attr ] {
  - Arguments [4] {
    Argument #0 [ Test\strrev(...) ]
    Argument #1 [ \strrev(...) ]
    Argument #2 [ \Test\Clazz::foo(...) ]
    Argument #3 [ self::foo(...) ]
  }
}
