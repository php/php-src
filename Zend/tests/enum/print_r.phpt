--TEST--
Enum print_r
--FILE--
<?php

enum Foo {
    case Bar;
}

enum IntFoo: int {
    case Bar = 42;
}

enum StringFoo: string {
    case Bar = 'Bar';
}

print_r(Foo::Bar);
print_r(IntFoo::Bar);
print_r(StringFoo::Bar);

?>
--EXPECT--
Foo Enum
(
    [name] => Bar
)
IntFoo Enum:int
(
    [name] => Bar
    [value] => 42
)
StringFoo Enum:string
(
    [name] => Bar
    [value] => Bar
)
