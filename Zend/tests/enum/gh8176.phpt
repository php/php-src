--TEST--
Enum object in property initializer
--FILE--
<?php

class AClass
{
    public $prop = AnEnum::Value;
}

enum AnEnum
{
    case Value;
}

var_dump(new AClass);

?>
--EXPECT--
object(AClass)#2 (1) {
  ["prop"]=>
  enum(AnEnum::Value)
}
