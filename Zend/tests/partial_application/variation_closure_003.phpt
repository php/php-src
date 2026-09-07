--TEST--
PFA variation: Closure::__invoke() with $this
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
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %svariation_closure_003.php 14 - 14

  - Bound Variables [1] {
      Variable #0 [ $a ]
  }

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
