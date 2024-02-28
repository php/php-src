--TEST--
Method returned callable, expected callable
--FILE--
<?php
class foo {
    public function bar() : callable {
        $test = "one";
        return function() use($test) : array {
            return array($test);
        };
    }
}

$baz = new foo();
var_dump($baz->bar());
?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["test"]=>
    string(3) "one"
  }
  ["this"]=>
  object(foo)#%d (0) {
  }
}
