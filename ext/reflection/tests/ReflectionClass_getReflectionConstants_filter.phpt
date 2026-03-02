--TEST--
ReflectionClass::getReflectionConstants() with $filter
--FILE--
<?php
class A {
    public const PUBLIC_CONST = 'BAR';
    public const ANOTHER_PUBLIC_CONST = 'BAZ';
    protected const PROTECTED_CONST = 'FOO';
    private const PRIVATE_CONST = 'QUOZ';
}

class B {
  public const PUBLIC_CONST = 'BAR';
  protected const ANOTHER_PROTECTED_CONST = 'BAZ';
  protected const PROTECTED_CONST = 'FOO';
  private const PRIVATE_CONST = 'QUOZ';
}

class C {
  public const PUBLIC_CONST = 'BAR';
  protected const PROTECTED_CONST = 'FOO';
  private const PRIVATE_CONST = 'QUOZ';
  private const ANOTHER_PRIVATE_CONST = 'BAZ';
}

$reflectionClassA = new ReflectionClass(A::class);
var_dump($reflectionClassA->getReflectionConstants(ReflectionClassConstant::IS_PUBLIC));

$reflectionClassB = new ReflectionClass(B::class);
var_dump($reflectionClassB->getReflectionConstants(ReflectionClassConstant::IS_PROTECTED));

$reflectionClassC = new ReflectionClass(C::class);
var_dump($reflectionClassC->getReflectionConstants(ReflectionClassConstant::IS_PRIVATE));
?>
--EXPECTF--
array(2) {
  [0]=>
  object(ReflectionClassConstant)#%d (%d) {
    ["name"]=>
    string(%d) "PUBLIC_CONST"
    ["class"]=>
    string(%d) "A"
  }
  [1]=>
  object(ReflectionClassConstant)#%d (%d) {
    ["name"]=>
    string(%d) "ANOTHER_PUBLIC_CONST"
    ["class"]=>
    string(%d) "A"
  }
}
array(2) {
  [0]=>
  object(ReflectionClassConstant)#%d (%d) {
    ["name"]=>
    string(%d) "ANOTHER_PROTECTED_CONST"
    ["class"]=>
    string(%d) "B"
  }
  [1]=>
  object(ReflectionClassConstant)#%d (%d) {
    ["name"]=>
    string(%d) "PROTECTED_CONST"
    ["class"]=>
    string(%d) "B"
  }
}
array(2) {
  [0]=>
  object(ReflectionClassConstant)#%d (%d) {
    ["name"]=>
    string(%d) "PRIVATE_CONST"
    ["class"]=>
    string(%d) "C"
  }
  [1]=>
  object(ReflectionClassConstant)#%d (%d) {
    ["name"]=>
    string(%d) "ANOTHER_PRIVATE_CONST"
    ["class"]=>
    string(%d) "C"
  }
}
