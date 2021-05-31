--TEST--
Enum value property has automatic type
--FILE--
<?php

enum IntEnum: int {
    case Foo = 0;
}

enum StringEnum: string {
    case Foo = 'Foo';
}

echo (new ReflectionProperty(IntEnum::class, 'value'))->getType() . "\n";
echo (new ReflectionProperty(StringEnum::class, 'value'))->getType() . "\n";

?>
--EXPECT--
int
string
