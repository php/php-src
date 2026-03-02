--TEST--
non-existent imported functions should not be looked up in the global table
--FILE--
<?php

namespace {
    function test() {
        echo "NO!";
    }
}
namespace foo {
    use function bar\test;
    test();
}

?>
--EXPECTF--
Fatal error: Uncaught Error: Call to undefined function bar\test() in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
