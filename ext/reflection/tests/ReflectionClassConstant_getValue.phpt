--TEST--
Test variations of getting constant values
--FILE--
<?php

/* Use separate classes to make sure that in-place constant updates don't interfere */
class A {
    const X = self::Y * 2;
    const Y = 1;
}
class B {
    const X = self::Y * 2;
    const Y = 1;
}
class C {
    const X = self::Y * 2;
    const Y = 1;
}

var_dump((new ReflectionClassConstant('A', 'X'))->getValue());
echo new ReflectionClassConstant('B', 'X');
echo new ReflectionClass('C');

?>
--EXPECTF--
int(2)
Constant [ public integer X ] { 2 }
Class [ <user> class C ] {
  @@ %s 12-15

  - Constants [2] {
    Constant [ public integer X ] { 2 }
    Constant [ public integer Y ] { 1 }
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
