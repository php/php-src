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

echo new ReflectionClass('just_constants');
?>
--EXPECTF--
Class [ <user> class just_constants ] {
  @@ %s %d-%d

  - Constants [5] {
    Constant [ public bool BOOLEAN_CONSTANT ] { 1 }
    Constant [ public null NULL_CONSTANT ] {  }
    Constant [ public string STRING_CONSTANT ] { This is a string }
    Constant [ public int INTEGER_CONSTANT ] { 1000 }
    Constant [ public float FLOAT_CONSTANT ] { 3.14159265 }
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
