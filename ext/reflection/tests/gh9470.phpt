--TEST--
GH-9470: ReflectionMethod constructor should not find private parent method
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
try {
    echo (string) new ReflectionMethod('B', 'privateMethod');
} catch(Throwable $e){
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Method [ <user, inherits A> public method publicMethod ] {
  @@ %s 5 - 5
}
Method [ <user, inherits A> protected method protectedMethod ] {
  @@ %s 6 - 6
}
Method B::privateMethod() does not exist
