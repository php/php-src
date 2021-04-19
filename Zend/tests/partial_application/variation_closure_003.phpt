--TEST--
Partial application variation closure __invoke with this
--FILE--
<?php
class Foo {
    public function bar() {
        return function($a, $b) {
            return [$this, func_get_args()];
        };
    }
}

$foo = new Foo;

$closure = $foo->bar();

$function = $closure->__invoke(1, ?);

echo (string) new ReflectionFunction($function);

var_dump($function(10));
?>
--EXPECTF--
Method [ <internal, prototype Closure> partial public method __invoke ] {

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
array(2) {
  [0]=>
  object(Foo)#1 (0) {
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(10)
  }
}

