--TEST--
ReflectionClass::getConstants() with $filter
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
var_dump($reflectionClassA->getConstants(ReflectionClassConstant::IS_PUBLIC));

$reflectionClassB = new ReflectionClass(B::class);
var_dump($reflectionClassB->getConstants(ReflectionClassConstant::IS_PROTECTED));

$reflectionClassC = new ReflectionClass(C::class);
var_dump($reflectionClassC->getConstants(ReflectionClassConstant::IS_PRIVATE));
?>
--EXPECTF--
array(%d) {
  ["PUBLIC_CONST"]=>
  string(%d) "BAR"
  ["ANOTHER_PUBLIC_CONST"]=>
  string(%d) "BAZ"
}
array(%d) {
  ["ANOTHER_PROTECTED_CONST"]=>
  string(%d) "BAZ"
  ["PROTECTED_CONST"]=>
  string(%d) "FOO"
}
array(%d) {
  ["PRIVATE_CONST"]=>
  string(%d) "QUOZ"
  ["ANOTHER_PRIVATE_CONST"]=>
  string(%d) "BAZ"
}
