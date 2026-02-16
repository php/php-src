--TEST--
PFA variation: parent
--FILE--
<?php
class Foo {
    public function method($a, $b, ...$c) {
        return function() {
            return $this;
        };
    }
}

$foo = new Foo();
$bar = $foo->method(10, ...);
$baz = $bar(20, ...);

var_dump($baz, $c = $baz(), $c() === $foo);
?>
--EXPECTF--
object(Closure)#%d (6) {
  ["name"]=>
  string(%d) "{closure:%s}"
  ["file"]=>
  string(%d) "%svariation_parent_001.php"
  ["line"]=>
  int(12)
  ["static"]=>
  array(2) {
    ["fn"]=>
    object(Closure)#%d (6) {
      ["name"]=>
      string(%d) "{closure:%s:%d}"
      ["file"]=>
      string(%d) "%s"
      ["line"]=>
      int(11)
      ["static"]=>
      array(1) {
        ["a"]=>
        int(10)
      }
      ["this"]=>
      object(Foo)#%d (0) {
      }
      ["parameter"]=>
      array(2) {
        ["$b"]=>
        string(10) "<required>"
        ["$c"]=>
        string(10) "<optional>"
      }
    }
    ["b"]=>
    int(20)
  }
  ["this"]=>
  object(Foo)#%d (0) {
  }
  ["parameter"]=>
  array(1) {
    ["$c"]=>
    string(10) "<optional>"
  }
}
object(Closure)#%d (4) {
  ["name"]=>
  string(25) "{closure:Foo::method():4}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(4)
  ["this"]=>
  object(Foo)#%d (0) {
  }
}
bool(true)
