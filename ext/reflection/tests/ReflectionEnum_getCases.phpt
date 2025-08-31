--TEST--
ReflectionEnum::getCases()
--FILE--
<?php

enum Enum_ {
    case Foo;
    case Bar;
    const Baz = self::Bar;
}

enum IntEnum: int {
  case Foo = 0;
  case Bar = 1;
  const Baz = self::Bar;
}

var_dump((new ReflectionEnum(Enum_::class))->getCases());
var_dump((new ReflectionEnum(IntEnum::class))->getCases());

?>
--EXPECT--
array(2) {
  [0]=>
  object(ReflectionEnumUnitCase)#2 (2) {
    ["name"]=>
    string(3) "Foo"
    ["class"]=>
    string(5) "Enum_"
  }
  [1]=>
  object(ReflectionEnumUnitCase)#3 (2) {
    ["name"]=>
    string(3) "Bar"
    ["class"]=>
    string(5) "Enum_"
  }
}
array(2) {
  [0]=>
  object(ReflectionEnumBackedCase)#2 (2) {
    ["name"]=>
    string(3) "Foo"
    ["class"]=>
    string(7) "IntEnum"
  }
  [1]=>
  object(ReflectionEnumBackedCase)#1 (2) {
    ["name"]=>
    string(3) "Bar"
    ["class"]=>
    string(7) "IntEnum"
  }
}
