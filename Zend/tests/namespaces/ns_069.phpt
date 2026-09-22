--TEST--
069: Include inside namespaced method
--FILE--
<?php

namespace foo;

class Test {
  static function f() {
    var_dump(__NAMESPACE__);
    include __DIR__ . '/ns_069.inc';
    var_dump(__NAMESPACE__);
  }
}

Test::f();

?>
--EXPECT--
string(3) "foo"
string(0) ""
string(3) "foo"
