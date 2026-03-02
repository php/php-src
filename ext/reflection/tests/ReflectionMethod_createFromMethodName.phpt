--TEST--
ReflectionMethod::createFromMethodName()
--FILE--
<?php

class Foo {
    public function bar() {}
}

class MyReflectionMethod extends ReflectionMethod {}

$m = MyReflectionMethod::createFromMethodName("Foo::bar");

var_dump($m);

try {
    $m = MyReflectionMethod::createFromMethodName("Foo::baz");
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
object(MyReflectionMethod)#%d (%d) {
  ["name"]=>
  string(3) "bar"
  ["class"]=>
  string(3) "Foo"
}
Method Foo::baz() does not exist
