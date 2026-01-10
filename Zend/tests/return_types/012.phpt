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
object(Closure)#%d (5) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
  ["static"]=>
  array(1) {
    ["test"]=>
    string(3) "one"
  }
  ["this"]=>
  object(foo)#%d (0) {
  }
}
