--TEST--
069: Include inside namespaced method
--FILE--
<?php

namespace foo;

class Test {
  static function f() {
    var_dump((binary)__NAMESPACE__);
    include __DIR__ . '/ns_069.inc';
    var_dump((binary)__NAMESPACE__);
  }
}

Test::f();

?>
===DONE===
--EXPECT--
string(3) "foo"
string(0) ""
string(3) "foo"
===DONE===
