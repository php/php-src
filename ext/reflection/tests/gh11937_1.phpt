--TEST--
GH-11937: Segfault in format_default_value due to unexpected enum/object
--FILE--
<?php

enum TestEnum {
    case One;
    case Two;
    const CASES = [self::One, self::Two];
}

var_dump(TestEnum::CASES);

require __DIR__ . '/gh11937_1.inc';

echo (new ReflectionFunction('test'))->getAttributes('Attr')[0];

?>
--EXPECT--
array(2) {
  [0]=>
  enum(TestEnum::One)
  [1]=>
  enum(TestEnum::Two)
}
Attribute [ Attr ] {
  - Arguments [1] {
    Argument #0 [ new \Foo(TestEnum::CASES) ]
  }
}
