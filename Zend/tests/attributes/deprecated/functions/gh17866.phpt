--TEST--
GH-17866 (zend_mm_heap corrupted error after upgrading from 8.4.3 to 8.4.4)
--FILE--
<?php

class Foo {
    #[Deprecated("xyzzy")]
    public function __invoke() {
        echo "In __invoke\n";
    }
}

$foo = new Foo;
$closure = Closure::fromCallable($foo);
$test = $closure->__invoke(...);

$rc = new ReflectionMethod($test, '__invoke');
var_dump($rc->getAttributes());

$test();

?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(10) "Deprecated"
  }
}

Deprecated: Method Foo::__invoke() is deprecated, xyzzy in %s on line %d
In __invoke
