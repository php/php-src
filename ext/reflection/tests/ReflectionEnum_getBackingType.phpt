--TEST--
ReflectionEnum::getBackingType()
--FILE--
<?php

enum Enum_ {}
enum IntEnum: int {}
enum StringEnum: string {}

function test(): string {}

var_dump((new ReflectionEnum(Enum_::class))->getBackingType());
echo (new ReflectionEnum(IntEnum::class))->getBackingType() . "\n";
echo (new ReflectionEnum(StringEnum::class))->getBackingType() . "\n";

?>
--EXPECT--
NULL
int
string
