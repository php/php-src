--TEST--
ReflectionEnum::getCases()
--FILE--
<?php

enum Enum_ {
    case Foo;
    const Bar = self::Foo;
}

enum IntEnum: int {
    case Foo = 0;
    const Bar = self::Foo;
}

function test(string $enumName, string $caseName) {
    try {
        $reflectionEnum = new ReflectionEnum($enumName);
        var_dump($reflectionEnum->getCase($caseName));
    } catch (Throwable $e) {
        echo get_class($e) . ': ' . $e->getMessage() . "\n";
    }
}

test(Enum_::class, 'Foo');
test(Enum_::class, 'Bar');
test(Enum_::class, 'Baz');

test(IntEnum::class, 'Foo');
test(IntEnum::class, 'Bar');
test(IntEnum::class, 'Baz');

?>
--EXPECT--
object(ReflectionEnumUnitCase)#2 (2) {
  ["name"]=>
  string(3) "Foo"
  ["class"]=>
  string(5) "Enum_"
}
ReflectionException: Enum_::Bar is not a case
ReflectionException: Case Enum_::Baz does not exist
object(ReflectionEnumBackedCase)#2 (2) {
  ["name"]=>
  string(3) "Foo"
  ["class"]=>
  string(7) "IntEnum"
}
ReflectionException: IntEnum::Bar is not a case
ReflectionException: Case IntEnum::Baz does not exist
