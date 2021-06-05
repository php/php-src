--TEST--
Partial application variation binding static
--FILE--
<?php
class Param {

    public function __toString() {
        return __CLASS__;
    }
}

class Foo {
    public static function method($a, $b) {
        printf("%s: %s, %s\n", get_called_class(), $a, $b);    
    }
}

class Bar extends Foo {

}

$closure = Bar::method(?, new Param);

$closure(1);

var_dump($closure);

$bound = $closure->bindTo(null, Foo::class);

$bound(1);

var_dump($bound);
?>
--EXPECTF--
Bar: 1, Param
object(Closure)#%d (2) {
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<required>"
  }
  ["args"]=>
  array(2) {
    ["a"]=>
    NULL
    ["b"]=>
    object(Param)#%d (0) {
    }
  }
}
Foo: 1, Param
object(Closure)#%d (2) {
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<required>"
  }
  ["args"]=>
  array(2) {
    ["a"]=>
    NULL
    ["b"]=>
    object(Param)#%d (0) {
    }
  }
}

