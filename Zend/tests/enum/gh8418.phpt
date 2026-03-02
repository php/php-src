--TEST--
GH-8418: Enum constant expression evaluation doesn't work with warmed cache
--FILE--
<?php

class ExampleClass
{
    public const EXAMPLE_CONST = 42;
}

enum ExampleEnum: int
{
    case ENUM_CASE = ExampleClass::EXAMPLE_CONST;
}

var_dump(ExampleEnum::ENUM_CASE->value);

?>
--EXPECT--
int(42)
