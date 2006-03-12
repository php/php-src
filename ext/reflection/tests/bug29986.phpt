--TEST--
Reflection Bug #29986 (Class constants won't work with predefined constants when using ReflectionClass)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
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
  @@ %s

  - Constants [5] {
    Constant [ boolean BOOLEAN_CONSTANT ] { }
    Constant [ null NULL_CONSTANT ] { }
    Constant [ string STRING_CONSTANT ] { }
    Constant [ integer INTEGER_CONSTANT ] { }
    Constant [ double FLOAT_CONSTANT ] { }
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
