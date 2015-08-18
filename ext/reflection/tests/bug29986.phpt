--TEST--
Reflection Bug #29986 (Class constants won't work with predefined constants when using ReflectionClass)
--INI--
precision=14
--FILE--
<?php
class just_constants
{
    const BOOLEAN_CONSTANT = true;
    const NULL_CONSTANT = null;
    const STRING_CONSTANT = 'This is a string';
    const INTEGER_CONSTANT = 1000;
    const FLOAT_CONSTANT = 3.14159265;
}

Reflection::export(new ReflectionClass('just_constants'));
?>
--EXPECTF--
Class [ <user> class just_constants ] {
  @@ %s %d-%d

  - Constants [5] {
    Constant [ boolean BOOLEAN_CONSTANT ] { 1 }
    Constant [ null NULL_CONSTANT ] {  }
    Constant [ string STRING_CONSTANT ] { This is a string }
    Constant [ integer INTEGER_CONSTANT ] { 1000 }
    Constant [ float FLOAT_CONSTANT ] { 3.14159265 }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
