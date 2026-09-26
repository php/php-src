--TEST--
ReflectionClassConstant::__toString() and class constant display
--FILE--
<?php

class Demo {
    public const IS_TRUE = true;
    public const IS_FALSE = false;
}

echo new ReflectionClassConstant(Demo::class, 'IS_TRUE');
echo new ReflectionClassConstant(Demo::class, 'IS_FALSE');

echo new ReflectionClass(Demo::class);

?>
--EXPECTF--
Constant [ public bool IS_TRUE ] { true }
Constant [ public bool IS_FALSE ] { false }
Class [ <user> class Demo ] {
  @@ %s %d-%d

  - Constants [2] {
    Constant [ public bool IS_TRUE ] { true }
    Constant [ public bool IS_FALSE ] { false }
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
