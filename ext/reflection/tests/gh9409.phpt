--TEST--
GH-9409: Private method is incorrectly dumped as "overwrites"
--FILE--
<?php

class A {
    private function privateMethod() {}
}

class C extends A {
    private function privateMethod() {}
}

echo (new ReflectionClass('A'))->getMethod('privateMethod');
echo (new ReflectionClass('C'))->getMethod('privateMethod');

?>
--EXPECTF--
Method [ <user> private method privateMethod ] {
  @@ %s %d - %d
}
Method [ <user> private method privateMethod ] {
  @@ %s %d - %d
}
