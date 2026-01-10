--TEST--
GH-9470: ReflectionMethod constructor finds private parent method
--FILE--
<?php

class A
{
    public function publicMethod() {}
    protected function protectedMethod() {}
    private function privateMethod() {}
}
class B extends A {}

echo (string) new ReflectionMethod('B', 'publicMethod');
echo (string) new ReflectionMethod('B', 'protectedMethod');
echo (string) new ReflectionMethod('B', 'privateMethod');

$r = new ReflectionClass('B');
echo (string) $r->getMethod('publicMethod');
echo (string) $r->getMethod('protectedMethod');
echo (string) $r->getMethod('privateMethod');

?>
--EXPECTF--
Method [ <user, inherits A> public method publicMethod ] {
  @@ %s 5 - 5
}
Method [ <user, inherits A> protected method protectedMethod ] {
  @@ %s 6 - 6
}
Method [ <user, inherits A> private method privateMethod ] {
  @@ %s 7 - 7
}
Method [ <user, inherits A> public method publicMethod ] {
  @@ %s 5 - 5
}
Method [ <user, inherits A> protected method protectedMethod ] {
  @@ %s 6 - 6
}
Method [ <user, inherits A> private method privateMethod ] {
  @@ %s 7 - 7
}
