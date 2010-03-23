--TEST--
Bug #45862 (get_class_vars is inconsistent with 'protected' and 'private' variables)
--FILE--
<?php

class Ancestor {
  function test() {
    var_dump(get_class_vars("Tester"));
    var_dump(Tester::$prot);
  }
}

class Tester extends Ancestor {
  static protected $prot = "protected var";
  static private $priv = "private var";
}

class Child extends Tester {
  function test() { var_dump(get_class_vars("Tester")); }
}

echo "\n From parent scope\n";
$parent = new Ancestor();
$parent->test();
echo "\n From child scope\n";
$child = new Child();
$child->test();

?>
--EXPECT--

 From parent scope
array(1) {
  ["prot"]=>
  string(13) "protected var"
}
string(13) "protected var"

 From child scope
array(1) {
  ["prot"]=>
  string(13) "protected var"
}
