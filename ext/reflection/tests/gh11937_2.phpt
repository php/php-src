--TEST--
GH-11937: Segfault in format_default_value due to unexpected enum/object
--FILE--
<?php

class Foo {}

const FOOS = [new Foo()];

var_dump(FOOS);

require __DIR__ . '/gh11937_2.inc';

echo (new ReflectionFunction('test'))->getAttributes('Attr')[0];

?>
--EXPECT--
array(1) {
  [0]=>
  object(Foo)#1 (0) {
  }
}
Attribute [ Attr ] {
  - Arguments [1] {
    Argument #0 [ FOOS ]
  }
}
